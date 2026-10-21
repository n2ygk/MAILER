#!/usr/bin/env python3
"""Convert line-printer output (including ASA carriage control) to Markdown."""

from __future__ import annotations

import argparse
import html
import sys
from pathlib import Path
from typing import Sequence


Page = list[list[str]]
Document = list[Page]


def detect_format(text: str) -> str:
  """Return ``asa`` when *text* looks like ASA carriage-control output."""
  lines = [line for line in text.split("\n") if line]
  if not lines:
    return "text"

  # ASA files reserve column one on virtually every record.  Requiring a
  # large majority avoids mistaking ordinary text whose lines happen to
  # begin with a space or a digit for carriage-controlled output.
  controls = {" ", "0", "-", "+", "1"}
  controlled = sum(line[0] in controls for line in lines)
  return "asa" if controlled / len(lines) >= 0.8 else "text"


def _ensure_row(page: Page, row: int) -> None:
  while len(page) <= row:
    page.append([])


def parse_asa(text: str) -> Document:
  """Parse ASA carriage-control records into pages of overstrike layers."""
  pages: Document = [[]]
  page = pages[0]
  row = -1
  records = text.split("\n")

  for line_number, record in enumerate(records, start=1):
    if line_number == 1:
      record = record.removeprefix("\ufeff")
    record = record.removesuffix("\r")

    # A final newline is a record separator, not an additional empty record.
    if not record and line_number == len(records):
      break

    if not record:
      control, content = " ", ""
    else:
      control, content = record[0], record[1:]

    if control == "1":
      if page:
        page = []
        pages.append(page)
      row = 0
    elif control == "+":
      row = max(row, 0)
    elif control in {" ", "0", "-"}:
      row += {" ": 1, "0": 2, "-": 3}[control]
    else:
      raise ValueError(
          f"line {line_number}: unsupported ASA carriage-control "
          f"character {control!r}"
      )

    _ensure_row(page, row)
    page[row].append(content.expandtabs(8))

  return pages


def parse_text(text: str) -> Document:
  """Parse text containing form feeds and carriage-return overstrikes."""
  text = text.removeprefix("\ufeff")
  pages: Document = []

  for page_text in text.split("\f"):
    page: Page = []
    records = page_text.split("\n")
    if records and records[-1] == "":
      records.pop()
    for record in records:
      layers = record.split("\r")
      # A CR immediately before LF/FF merely returns the print head.
      if len(layers) > 1 and layers[-1] == "":
        layers.pop()
      page.append([layer.expandtabs(8) for layer in layers] or [""])
    pages.append(page)

  return pages


def _flatten_overstrikes(layers: list[str]) -> str:
  """Merge overstruck text into GitHub-safe inline HTML."""
  width = max((len(layer) for layer in layers), default=0)
  styled_characters: list[tuple[str, bool, bool]] = []

  for column in range(width):
    characters = [layer[column] for layer in layers if column < len(layer)]
    visible = [character for character in characters if character not in {" ", "_"}]
    character = visible[-1] if visible else "_" if "_" in characters else " "
    underlined = character != "_" and "_" in characters
    bold = character != " " and characters.count(character) > 1
    styled_characters.append((character, underlined, bold))

  # Group adjacent characters with the same styling to keep generated files
  # compact and easy to inspect as source.
  output: list[str] = []
  index = 0
  while index < len(styled_characters):
    character, underlined, bold = styled_characters[index]
    end = index + 1
    while end < len(styled_characters):
      _, next_underlined, next_bold = styled_characters[end]
      if (next_underlined, next_bold) != (underlined, bold):
        break
      end += 1

    text = html.escape("".join(item[0] for item in styled_characters[index:end]))
    if underlined:
      text = f"<u>{text}</u>"
    if bold:
      text = f"<strong>{text}</strong>"
    output.append(text)
    index = end

  return "".join(output).rstrip()


def render_markdown(document: Document, title: str = "Line printer output") -> str:
  """Render parsed pages as GitHub-flavored Markdown with embedded HTML."""
  output = [f"# {html.escape(title)}", ""]

  for page_number, page in enumerate(document, start=1):
    if page_number > 1:
      output.extend(["", "---", ""])
    output.extend([f"<!-- Page {page_number} -->", "", "<pre>"])
    output.extend(_flatten_overstrikes(layers or [""]) for layers in page)
    output.append("</pre>")

  output.append("")
  return "\n".join(output)


def convert_lpt(
    text: str,
    input_format: str = "auto",
    title: str = "Line printer output",
) -> str:
  """Convert line-printer *text* to Markdown."""
  if input_format == "auto":
    input_format = detect_format(text)
  if input_format == "asa":
    document = parse_asa(text)
  elif input_format == "text":
    document = parse_text(text)
  else:
    raise ValueError(f"unknown input format: {input_format}")
  return render_markdown(document, title)


def convert_asa_with_overstrike(asa_text: str) -> str:
  """Convert ASA text to Markdown (backward-compatible programmatic API)."""
  return convert_lpt(asa_text, input_format="asa")


def _parser() -> argparse.ArgumentParser:
  parser = argparse.ArgumentParser(
      description=(
          "Convert an .lpt line-printer file to GitHub-renderable Markdown, "
          "preserving ASA carriage control, form feeds, and overstrikes."
      )
  )
  parser.add_argument("input", help="input .lpt file, or - for standard input")
  parser.add_argument(
      "output",
      nargs="?",
      help="output .md file (default: input name with an .md suffix)",
  )
  parser.add_argument(
      "-o",
      "--output",
      dest="output_option",
      metavar="FILE",
      help="write to FILE instead of using the positional output argument",
  )
  parser.add_argument(
      "--format",
      choices=("auto", "asa", "text"),
      default="auto",
      dest="input_format",
      help="input format (default: auto-detect)",
  )
  parser.add_argument(
      "--encoding",
      default="utf-8",
      help="input character encoding (default: utf-8)",
  )
  parser.add_argument(
      "--title",
      help="Markdown document title (default: input file name)",
  )
  return parser


def main(argv: Sequence[str] | None = None) -> int:
  """Run the command-line converter."""
  parser = _parser()
  args = parser.parse_args(argv)

  if args.output is not None and args.output_option is not None:
    parser.error("output may be given either positionally or with --output, not both")
  if args.output_option is not None:
    args.output = args.output_option

  if args.input == "-":
    if args.output is None:
      args.output = "-"
    try:
      text = sys.stdin.buffer.read().decode(args.encoding)
    except (LookupError, UnicodeDecodeError) as error:
      parser.error(str(error))
    default_title = "Line printer output"
  else:
    input_path = Path(args.input)
    if input_path.suffix.lower() != ".lpt":
      parser.error(f"input file must have an .lpt extension: {input_path}")
    try:
      with input_path.open(encoding=args.encoding, newline="") as input_file:
        text = input_file.read()
    except (OSError, LookupError, UnicodeError) as error:
      parser.error(f"cannot read {input_path}: {error}")
    if args.output is None:
      args.output = str(input_path.with_suffix(".md"))
    default_title = input_path.name

  title = args.title if args.title is not None else default_title
  try:
    rendered = convert_lpt(text, args.input_format, title)
  except ValueError as error:
    parser.error(str(error))

  if args.output == "-":
    sys.stdout.write(rendered)
    return 0

  output_path = Path(args.output)
  if output_path.suffix.lower() not in {".md", ".markdown"}:
    parser.error(f"output file must have an .md or .markdown extension: {output_path}")
  if args.input != "-" and output_path.resolve() == Path(args.input).resolve():
    parser.error("input and output paths must be different")
  try:
    with output_path.open("w", encoding="utf-8", newline="\n") as output_file:
      output_file.write(rendered)
  except OSError as error:
    parser.error(f"cannot write {output_path}: {error}")
  return 0


if __name__ == "__main__":
  raise SystemExit(main())
