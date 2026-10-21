#!/usr/bin/env python3
"""Reconstruct IBM VM/CMS UPDATE history as a Git branch.

MAILER stores original, sequenced source files in ``mailer-base`` (and a
few later base files in ``mailer-updates``).  An AUX file names update decks
in newest-first order; CMS UPDATE consequently applies its entries from the
bottom upward.  This program reproduces that process as a new orphan branch
in the existing Git repository, grouping changes by update level.

The file checked into Git has the eight-column CMS sequence field removed,
which makes the result pleasant to read and diff on modern systems.  The
sequence fields are retained internally while applying the updates.
"""

from __future__ import annotations

import argparse
import dataclasses
import datetime as dt
import os
from pathlib import Path
import re
import subprocess
import sys
import tempfile
from typing import Iterable, Sequence


AUX_ENTRY = re.compile(r"^(R\d{6}[A-Z]?)\s+(.*\S)?\s*$", re.IGNORECASE)
CONTROL = re.compile(r"^\./\s+([A-Z*])(?:\s+(.*?))?\s*$", re.IGNORECASE)
TIMESTAMP = re.compile(r"(\d{2}/\d{2}/\d{2}\s+\d{2}:\d{2}:\d{2})\s*$")
SEQUENCE = re.compile(r"^\d{8}$")


class UpdateError(RuntimeError):
    """An UPDATE deck cannot be applied to the selected source."""


@dataclasses.dataclass(frozen=True)
class Record:
    text: str
    sequence: int


@dataclasses.dataclass(frozen=True)
class AuxEntry:
    update_id: str
    description: str
    deck: Path


@dataclasses.dataclass
class Chain:
    aux: Path
    base: Path
    entries: list[AuxEntry]
    records: list[Record]


@dataclasses.dataclass
class FileHistory:
    output: str
    base: Path
    entries: list[AuxEntry]
    records: list[Record]


def read_lines(path: Path) -> list[str]:
    """Read the repository's byte-oriented CMS files without newline loss."""
    return path.read_bytes().decode("latin-1").splitlines()


def parse_source(path: Path, *, allow_unsequenced: bool = False) -> list[Record]:
    lines = read_lines(path)
    has_sequence = [len(line) >= 80 and bool(SEQUENCE.fullmatch(line[72:80])) for line in lines]
    if lines and all(has_sequence):
        records = [Record(line[:72].rstrip(), int(line[72:80])) for line in lines]
    elif allow_unsequenced and not any(has_sequence):
        # The only useful operation on this provisional numbering is an
        # immediately following ./ S, which replaces every value.
        records = [Record(line.rstrip(), number * 1000) for number, line in enumerate(lines, 1)]
    else:
        problem = has_sequence.index(False) + 1 if False in has_sequence else 1
        raise UpdateError(f"{path}:{problem}: no numeric sequence in columns 73-80")
    if not records:
        raise UpdateError(f"{path}: empty source")
    return records


def parse_aux(path: Path, updates_dir: Path) -> list[AuxEntry]:
    raw: list[tuple[str, str]] = []
    current: int | None = None
    for line in read_lines(path):
        match = AUX_ENTRY.match(line)
        if match:
            update_id = match.group(1).upper()
            raw.append((update_id, (match.group(2) or "").strip()))
            current = len(raw) - 1
        elif line.startswith("*") and current is not None:
            continuation = line[1:].strip()
            if continuation:
                update_id, description = raw[current]
                raw[current] = (update_id, f"{description} {continuation}".strip())

    stem = path.name.split(".", 1)[0]
    entries: list[AuxEntry] = []
    # AUX lists are newest first; UPDATE applies them bottom-to-top.
    for update_id, description in reversed(raw):
        deck = updates_dir / f"{stem}.{update_id.lower()}"
        if not deck.is_file():
            raise UpdateError(f"{path}: referenced deck not found: {deck.name}")
        entries.append(AuxEntry(update_id, description, deck))
    return entries


def _index(records: Sequence[Record], sequence: int, deck: Path) -> int:
    positions = [i for i, record in enumerate(records) if record.sequence == sequence]
    if len(positions) != 1:
        reason = "not found" if not positions else "is duplicated"
        raise UpdateError(f"{deck}: sequence {sequence:08d} {reason}")
    return positions[0]


def _new_records(lines: Iterable[str], start: int, increment: int) -> list[Record]:
    result: list[Record] = []
    for offset, line in enumerate(lines):
        # UPDATE records are fixed at 80 columns.  Their existing sequence
        # fields are ignored when a '$ start increment' clause is present.
        text = line[:72].rstrip() if len(line) > 72 else line.rstrip()
        result.append(Record(text, start + offset * increment))
    return result


def apply_deck(records: Sequence[Record], deck: Path) -> tuple[list[Record], dt.datetime | None]:
    result = list(records)
    lines = read_lines(deck)
    index = 0
    timestamp: dt.datetime | None = None

    while index < len(lines):
        line = lines[index]
        match = CONTROL.match(line)
        if not match:
            raise UpdateError(f"{deck}:{index + 1}: data record without a control statement")

        operation = match.group(1).upper()
        argument_text = match.group(2) or ""
        # The MAILER decks append a human timestamp after the formal control
        # fields; it is metadata, not an UPDATE argument.
        argument_text = TIMESTAMP.sub("", argument_text).rstrip()
        arguments = argument_text.split()
        stamp_match = TIMESTAMP.search(line)
        if stamp_match and timestamp is None:
            timestamp = dt.datetime.strptime(stamp_match.group(1), "%m/%d/%y %H:%M:%S")

        next_control = index + 1
        while next_control < len(lines) and not CONTROL.match(lines[next_control]):
            next_control += 1
        data = lines[index + 1 : next_control]

        if operation == "*":
            if data:
                raise UpdateError(f"{deck}:{index + 1}: comment followed by data records")
        elif operation == "S":
            if data:
                raise UpdateError(f"{deck}:{index + 1}: sequence statement followed by data")
            start = int(arguments[0]) if arguments else 1000
            increment = int(arguments[1]) if len(arguments) > 1 else start
            result = [Record(record.text, start + i * increment) for i, record in enumerate(result)]
        elif operation in {"I", "D", "R"}:
            if not arguments:
                raise UpdateError(f"{deck}:{index + 1}: missing sequence number")
            try:
                separator = arguments.index("$")
            except ValueError:
                separator = len(arguments)
            old = [int(value) for value in arguments[:separator]]
            new = [int(value) for value in arguments[separator + 1 : separator + 3]]

            if operation == "I":
                if len(old) != 1 or not data:
                    raise UpdateError(f"{deck}:{index + 1}: malformed insert")
                position = _index(result, old[0], deck) + 1
                start = new[0] if new else old[0] + 1
                increment = new[1] if len(new) > 1 else 1
                result[position:position] = _new_records(data, start, increment)
            else:
                first = _index(result, old[0], deck)
                last = _index(result, old[1] if len(old) > 1 else old[0], deck)
                if last < first:
                    raise UpdateError(f"{deck}:{index + 1}: reversed sequence range")
                if operation == "D":
                    if data:
                        raise UpdateError(f"{deck}:{index + 1}: delete followed by data")
                    del result[first : last + 1]
                else:
                    if not data:
                        raise UpdateError(f"{deck}:{index + 1}: replace has no data")
                    start = new[0] if new else old[0]
                    increment = new[1] if len(new) > 1 else 1
                    result[first : last + 1] = _new_records(data, start, increment)
        else:
            raise UpdateError(f"{deck}:{index + 1}: unsupported operation {operation}")

        index = next_control

    return result, timestamp


def render(records: Sequence[Record]) -> str:
    return "".join(f"{record.text}\n" for record in records)


def _sequenced_sources(
    base_dir: Path, updates_dir: Path, stem: str, *, allow_unsequenced: bool
) -> list[Path]:
    candidates: list[Path] = []
    for directory in (base_dir, updates_dir):
        for path in sorted(directory.glob(f"{stem}.*")):
            suffix = path.name.split(".", 1)[1].lower()
            if suffix.startswith("aux") or suffix.startswith("r2"):
                continue
            try:
                parse_source(path, allow_unsequenced=allow_unsequenced)
            except (UpdateError, UnicodeError):
                continue
            candidates.append(path)
    return candidates


def _candidate_rank(aux: Path, candidate: Path) -> tuple[int, str]:
    aux_suffix = aux.name.split(".", 1)[1].lower()
    file_suffix = candidate.name.split(".", 1)[1].lower()
    preferences = {
        "auxexcr2": ["$exec", "$mailer"],
        "auxhlpr2": ["$helpcms"],
        "auxr2$h": ["$h"],
        "auxw3$h": ["$h"],
        "auxr2$a": ["$aux"],
    }.get(aux_suffix, ["assemble", "macro", "copy", "$c", "$h", "$exec", "$mailer"])
    try:
        rank = preferences.index(file_suffix)
    except ValueError:
        rank = len(preferences)
    return rank, candidate.name


def _allowed_suffixes(aux: Path) -> set[str] | None:
    aux_suffix = aux.name.split(".", 1)[1].lower()
    return {
        "auxexcr2": {"$exec", "$mailer", "exec"},
        "auxhlpr2": {"$helpcms"},
        "auxr2$h": {"$h"},
        "auxw3$h": {"$h"},
        "auxr2$a": {"$aux"},
    }.get(aux_suffix)


def _starts_with_sequence(deck: Path) -> bool:
    for line in read_lines(deck):
        match = CONTROL.match(line)
        if match:
            return match.group(1).upper() == "S"
    return False


def discover_chain(aux: Path, base_dir: Path, updates_dir: Path) -> Chain:
    entries = parse_aux(aux, updates_dir)
    stem = aux.name.split(".", 1)[0]
    allow_unsequenced = bool(entries and _starts_with_sequence(entries[0].deck))
    allowed_suffixes = _allowed_suffixes(aux)
    successes: list[tuple[Path, list[Record]]] = []
    failures: list[str] = []
    for candidate in _sequenced_sources(
        base_dir, updates_dir, stem, allow_unsequenced=allow_unsequenced
    ):
        candidate_suffix = candidate.name.split(".", 1)[1].lower()
        if allowed_suffixes is not None and candidate_suffix not in allowed_suffixes:
            continue
        records = parse_source(candidate, allow_unsequenced=allow_unsequenced)
        try:
            for entry in entries:
                records, _ = apply_deck(records, entry.deck)
        except UpdateError as exc:
            failures.append(f"{candidate.name}: {exc}")
        else:
            successes.append((candidate, records))
    if not successes:
        detail = f" ({'; '.join(failures)})" if failures else ""
        raise UpdateError(f"{aux}: no applicable sequenced base{detail}")
    successes.sort(key=lambda item: _candidate_rank(aux, item[0]))
    base, final_records = successes[0]
    return Chain(aux, base, entries, final_records)


def output_filename(base: Path) -> str:
    name = base.name
    stem, suffix = name.split(".", 1)
    if suffix.startswith("$"):
        suffix = suffix[1:]
    return f"{stem}.{suffix}"


def _git(
    repository: Path,
    *arguments: str,
    env: dict[str, str] | None = None,
    input_data: bytes | None = None,
    check: bool = True,
) -> subprocess.CompletedProcess[bytes]:
    return subprocess.run(
        ["git", *arguments], cwd=repository, env=env, input=input_data,
        check=check, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
    )


def _entry_key(entry: AuxEntry) -> tuple[int, int, str]:
    match = re.fullmatch(r"R(\d{6})([A-Z]?)", entry.update_id)
    if not match:
        raise UpdateError(f"invalid update identifier: {entry.update_id}")
    suffix = match.group(2)
    # An S deck establishes numbering before the ordinary deck at the same
    # level (MAILLEX and MAILLRP use this pattern).
    suffix_order = -1 if suffix == "S" else (0 if not suffix else 1)
    return int(match.group(1)), suffix_order, suffix


def _update_level(entry: AuxEntry) -> str:
    match = re.match(r"R\d{6}", entry.update_id)
    assert match
    return match.group(0)


def collect_histories(
    base_dir: Path, updates_dir: Path
) -> tuple[dict[str, FileHistory], list[str]]:
    chains: list[Chain] = []
    skipped: list[str] = []
    release_header_aux: list[Path] = []
    for aux in sorted(updates_dir.glob("*.aux*")):
        # AUXW3$H supplies the header deck for the R203013 compiler-upgrade
        # release.  It depends on sequence IDs introduced by the ordinary
        # header AUX, so merge it into that history after base discovery.
        if ".auxw3" in aux.name.lower():
            release_header_aux.append(aux)
            continue
        try:
            chains.append(discover_chain(aux, base_dir, updates_dir))
        except UpdateError as exc:
            skipped.append(str(exc))

    histories: dict[str, FileHistory] = {}
    for chain in chains:
        output = output_filename(chain.base)
        existing = histories.get(output)
        if existing is None:
            allow_unsequenced = bool(
                chain.entries and _starts_with_sequence(chain.entries[0].deck)
            )
            histories[output] = FileHistory(
                output, chain.base, list(chain.entries),
                parse_source(chain.base, allow_unsequenced=allow_unsequenced),
            )
        elif existing.base.resolve() != chain.base.resolve():
            raise UpdateError(
                f"output collision for {output}: {existing.base} and {chain.base}"
            )
        else:
            # The same base can have control-file-specific alternatives.  In
            # particular FORWARD $MAILER has both an obsolete AUXR2 chain and
            # the active AUXEXCR2 chain; they are not cumulative (they replace
            # some of the same sequence IDs).  Keep the first, preferred chain
            # selected by the sorted AUX filenames and report the alternative.
            skipped.append(
                f"{chain.aux}: alternate chain for {output}; "
                f"using {existing.entries[0].deck.name if existing.entries else existing.base.name}"
            )

    for aux in release_header_aux:
        history = histories.get("mailblok.h")
        if history is None:
            raise UpdateError(f"{aux}: mailblok.h history is unavailable")
        history.entries.extend(parse_aux(aux, updates_dir))

    # Recheck merged release/filetype streams from the original base.
    for history in histories.values():
        history.entries.sort(key=_entry_key)
        records = list(history.records)
        for entry in history.entries:
            records, _ = apply_deck(records, entry.deck)
    return histories, skipped


def _archive_environment(index_file: Path, timestamp: dt.datetime) -> dict[str, str]:
    environment = os.environ.copy()
    value = timestamp.strftime("%Y-%m-%dT%H:%M:%S +0000")
    environment.update(
        {
            "GIT_INDEX_FILE": str(index_file),
            "GIT_AUTHOR_NAME": "MAILER archive",
            "GIT_AUTHOR_EMAIL": "mailer-archive@localhost",
            "GIT_COMMITTER_NAME": "MAILER archive",
            "GIT_COMMITTER_EMAIL": "mailer-archive@localhost",
            "GIT_AUTHOR_DATE": value,
            "GIT_COMMITTER_DATE": value,
        }
    )
    return environment


def _write_blob(repository: Path, content: str) -> str:
    result = _git(repository, "hash-object", "-w", "--stdin", input_data=content.encode("latin-1"))
    return result.stdout.decode().strip()


def _stage_blob(repository: Path, environment: dict[str, str], path: str, blob: str) -> None:
    _git(
        repository, "update-index", "--add", "--cacheinfo", f"100644,{blob},{path}",
        env=environment,
    )


def _make_commit(
    repository: Path,
    environment: dict[str, str],
    message: str,
    parent: str | None,
) -> str:
    tree = _git(repository, "write-tree", env=environment).stdout.decode().strip()
    arguments = ["commit-tree", tree]
    if parent:
        arguments.extend(["-p", parent])
    result = _git(
        repository, *arguments, env=environment,
        input_data=(message.rstrip() + "\n").encode("utf-8"),
    )
    return result.stdout.decode().strip()


def _deck_timestamp(deck: Path) -> dt.datetime | None:
    for line in read_lines(deck):
        match = TIMESTAMP.search(line)
        if match:
            return dt.datetime.strptime(match.group(1), "%m/%d/%y %H:%M:%S")
    return None


def _snapshot_files(updates_dir: Path, histories: dict[str, FileHistory]) -> list[Path]:
    """Return readable final snapshots whose sequenced bases are unavailable."""
    names = (
        "ccomp.exec",
        "mailer.exec",
        "mailexec.exec",
        "mailgen.exec",
        "mailmod1.exec",
        "mailmod2.exec",
    )
    return [
        updates_dir / name for name in names
        if name not in histories and (updates_dir / name).is_file()
    ]


def build_branch(
    repository: Path,
    branch: str,
    base_dir: Path,
    updates_dir: Path,
    *,
    replace: bool = False,
) -> tuple[str, dict[str, FileHistory], list[str], int, int]:
    repository = repository.resolve()
    inside = _git(repository, "rev-parse", "--is-inside-work-tree", check=False)
    if inside.returncode != 0:
        raise UpdateError(f"not a Git working tree: {repository}")
    branch_ref = f"refs/heads/{branch}"
    existing_ref = _git(
        repository, "show-ref", "--verify", "--hash", branch_ref, check=False
    )
    if existing_ref.returncode == 0 and not replace:
        raise UpdateError(f"branch already exists: {branch}")
    if _git(repository, "check-ref-format", "--branch", branch, check=False).returncode != 0:
        raise UpdateError(f"invalid branch name: {branch}")

    histories, skipped = collect_histories(base_dir, updates_dir)
    snapshots = _snapshot_files(updates_dir, histories)
    groups: dict[str, list[tuple[FileHistory, AuxEntry]]] = {}
    for history in histories.values():
        for entry in history.entries:
            groups.setdefault(_update_level(entry), []).append((history, entry))

    all_dates = [
        stamp for history in histories.values() for entry in history.entries
        if (stamp := _deck_timestamp(entry.deck)) is not None
    ]
    base_date = min(all_dates, default=dt.datetime(1988, 1, 1)) - dt.timedelta(seconds=1)

    descriptor, index_name = tempfile.mkstemp(prefix="mailer-git-index-", dir="/tmp")
    os.close(descriptor)
    index_path = Path(index_name)
    index_path.unlink()
    environment = _archive_environment(index_path, base_date)
    try:
        _git(repository, "read-tree", "--empty", env=environment)
        intro = (
            "# Reconstructed MAILER sources\n\n"
            "This orphan branch was generated from `mailer-base` and the CMS "
            "UPDATE/AUX decks in `mailer-updates`. CMS sequence columns 73-80 "
            "were used for reconstruction and removed from the readable files "
            "under `src/`. AUX entries were applied bottom-to-top.\n"
        )
        if skipped:
            intro += (
                "\n## Chains not reconstructed\n\n"
                "These chains lacked an applicable original base, were optional, "
                "or conflicted with the active control-file chain:\n\n"
                + "".join(f"- `{message}`\n" for message in skipped)
            )
        if snapshots:
            intro += (
                "\n## Snapshot-only files\n\n"
                "The following readable final files are present, but their "
                "earlier commits cannot be reconstructed because the required "
                "sequenced base is absent. They are added in the final archival "
                "commit: "
                + ", ".join(f"`src/{path.name}`" for path in snapshots)
                + ".\n"
            )
        _stage_blob(repository, environment, "README.md", _write_blob(repository, intro))
        for output, history in sorted(histories.items()):
            blob = _write_blob(repository, render(history.records))
            _stage_blob(repository, environment, f"src/{output}", blob)
        parent = _make_commit(
            repository, environment,
            f"Import {len(histories)} recoverable MAILER base files", None,
        )

        commit_count = 1
        previous_date = base_date
        for level in sorted(groups, key=lambda value: int(value[1:])):
            actions = groups[level]
            descriptions: list[str] = []
            dates: list[dt.datetime] = []
            # Preserve each file's AUX order for same-level suffix decks.
            actions.sort(key=lambda item: item[0].entries.index(item[1]))
            changed: set[str] = set()
            for history, entry in actions:
                history.records, stamp = apply_deck(history.records, entry.deck)
                changed.add(history.output)
                if entry.description and entry.description not in descriptions:
                    descriptions.append(entry.description)
                if stamp:
                    dates.append(stamp)
            for output in sorted(changed):
                history = histories[output]
                blob = _write_blob(repository, render(history.records))
                _stage_blob(repository, environment, f"src/{output}", blob)

            timestamp = max(dates, default=previous_date + dt.timedelta(seconds=1))
            if timestamp <= previous_date:
                timestamp = previous_date + dt.timedelta(seconds=1)
            previous_date = timestamp
            environment = _archive_environment(index_path, timestamp)
            summary = descriptions[0] if descriptions else "Apply CMS UPDATE level"
            message = f"{level}: {summary}"
            if len(descriptions) > 1:
                message += "\n\n" + "\n".join(f"- {item}" for item in descriptions[1:])
            parent = _make_commit(repository, environment, message, parent)
            commit_count += 1

        if snapshots:
            for path in snapshots:
                content = path.read_bytes().decode("latin-1")
                if content and not content.endswith("\n"):
                    content += "\n"
                _stage_blob(
                    repository, environment, f"src/{path.name}",
                    _write_blob(repository, content),
                )
            timestamp = previous_date + dt.timedelta(seconds=1)
            environment = _archive_environment(index_path, timestamp)
            parent = _make_commit(
                repository, environment,
                "Import final snapshots whose update bases are unavailable",
                parent,
            )
            commit_count += 1

        expected = (
            existing_ref.stdout.decode().strip()
            if existing_ref.returncode == 0
            else "0" * 40
        )
        _git(repository, "update-ref", branch_ref, parent, expected)
    finally:
        index_path.unlink(missing_ok=True)
    return parent, histories, skipped, commit_count, len(snapshots)


def parse_args(arguments: Sequence[str] | None = None) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--repository", type=Path, default=Path("."), help="existing Git repository")
    parser.add_argument("--branch", default="codex/mailer-reconstructed", help="new orphan branch")
    parser.add_argument("--replace", action="store_true", help="atomically replace an existing generated branch")
    parser.add_argument("--base-dir", type=Path, default=Path("mailer-base"))
    parser.add_argument("--updates-dir", type=Path, default=Path("mailer-updates"))
    return parser.parse_args(arguments)


def main(arguments: Sequence[str] | None = None) -> int:
    options = parse_args(arguments)
    try:
        tip, histories, skipped, commit_count, snapshot_count = build_branch(
            options.repository, options.branch, options.base_dir, options.updates_dir,
            replace=options.replace,
        )
    except (UpdateError, OSError, subprocess.CalledProcessError) as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 1

    print(
        f"Created {options.branch} at {tip[:12]}: "
        f"{len(histories) + snapshot_count} files, {commit_count} commits"
    )
    if skipped:
        print(f"Unreconstructed {len(skipped)} chain(s):", file=sys.stderr)
        for message in skipped:
            print(f"  {message}", file=sys.stderr)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
