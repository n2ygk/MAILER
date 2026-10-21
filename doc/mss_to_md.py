#!/usr/bin/env python3
"""Convert legacy Scribe manuscript (.mss) files to GitHub Markdown."""

from __future__ import annotations

import argparse
import html
import re
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence


class ScribeError(ValueError):
  """Raised when a Scribe manuscript cannot be parsed."""


@dataclass
class Text:
  value: str


@dataclass
class Command:
  name: str
  argument: str | None = None


@dataclass
class Environment:
  name: str
  children: list[Node]


Node = Text | Command | Environment


class Parser:
  """Parse Scribe commands, balanced delimiters, and environments."""

  DELIMITERS = {"(": ")", "[": "]", "{": "}", "<": ">"}

  def __init__(self, source: str):
    self.source = source
    self.position = 0

  def parse(self, stop_environment: str | None = None) -> list[Node]:
    nodes: list[Node] = []
    text_start = self.position

    while self.position < len(self.source):
      if self.source[self.position] != "@":
        self.position += 1
        continue

      if text_start < self.position:
        nodes.append(Text(self.source[text_start:self.position]))

      if self.position + 1 >= len(self.source):
        nodes.append(Text("@"))
        self.position += 1
        text_start = self.position
        continue

      following = self.source[self.position + 1]
      if following == "@":
        nodes.append(Text("@"))
        self.position += 2
        text_start = self.position
        continue
      if following == "\\":
        nodes.append(Command("\\"))
        self.position += 2
        text_start = self.position
        continue
      if not (following.isalnum() or following in "+-"):
        # Scribe quotes punctuation by prefixing it with an at-sign.  @^ is
        # the tab character used by a few fixed-format examples.
        nodes.append(Text("\t" if following == "^" else following))
        self.position += 2
        text_start = self.position
        continue

      self.position += 1
      name_start = self.position
      while self.position < len(self.source):
        character = self.source[self.position]
        if not (character.isalnum() or character in "_+-"):
          break
        self.position += 1
      name = self.source[name_start:self.position]
      argument = None
      if (
          self.position < len(self.source)
          and self.source[self.position] in self.DELIMITERS
      ):
        argument = self._read_argument()

      lowered = name.lower()
      if lowered == "begin":
        if argument is None:
          self._error("@begin requires an environment name")
        environment = self._environment_name(argument)
        children = self.parse(environment)
        nodes.append(Environment(environment, children))
      elif lowered == "end":
        if argument is None:
          self._error("@end requires an environment name")
        environment = self._environment_name(argument)
        if stop_environment is None:
          self._error(f"unexpected @end({environment})")
        if environment != stop_environment:
          self._error(
              f"expected @end({stop_environment}), found @end({environment})"
          )
        return nodes
      else:
        nodes.append(Command(lowered, argument))

      text_start = self.position

    if text_start < self.position:
      nodes.append(Text(self.source[text_start:self.position]))
    if stop_environment is not None:
      self._error(f"missing @end({stop_environment})")
    return nodes

  def _read_argument(self) -> str:
    opener = self.source[self.position]
    closer = self.DELIMITERS[opener]
    start = self.position + 1
    self.position += 1
    depth = 1

    while self.position < len(self.source):
      character = self.source[self.position]
      if character == "@" and self.position + 1 < len(self.source):
        if self.source[self.position + 1] == "@":
          self.position += 2
          continue
        if self.source[self.position + 1] == closer:
          self.position += 2
          continue
      if character == opener:
        depth += 1
      elif character == closer:
        depth -= 1
        if depth == 0:
          argument = self.source[start:self.position]
          self.position += 1
          return argument
      self.position += 1

    self._error(f"unterminated {opener}{closer} argument")
    raise AssertionError("unreachable")

  @staticmethod
  def _environment_name(argument: str) -> str:
    return re.split(r"[\s,]", argument.strip(), maxsplit=1)[0].lower()

  def _error(self, message: str) -> None:
    line = self.source.count("\n", 0, self.position) + 1
    raise ScribeError(f"line {line}: {message}")


class Renderer:
  """Render a parsed Scribe document as GitHub-flavored Markdown."""

  STRUCTURAL_COMMANDS = {
      "majorheading": 1,
      "section": 2,
      "heading": 2,
      "subsection": 3,
      "paragraph": 4,
  }
  IGNORED_COMMANDS = {
      "blankpage",
      "bar",
      "case",
      "cboff",
      "cbon",
      "comment",
      "device",
      "fontfamily",
      "hinge",
      "index",
      "make",
      "modify",
      "pagefooting",
      "pageheading",
      "sendend",
      "set",
      "style",
      "tabclear",
      "use",
  }
  TRANSPARENT_ENVIRONMENTS = {
      "figure",
      "multiple",
      "researchcredit",
      "text",
      "titlebox",
      "titlepage",
      "transparent",
  }

  def __init__(
      self,
      *,
      strict: bool = False,
      file_date: str | None = None,
      title: str | None = None,
  ):
    self.strict = strict
    self.file_date = file_date
    self.title = title
    self.macros: dict[str, list[Node]] = {}
    self.footnotes: list[str] = []
    self.unknown_commands: set[str] = set()
    self.unknown_environments: set[str] = set()

  def render(self, nodes: list[Node]) -> str:
    self._collect_macros(nodes)
    body = self._render_nodes(nodes).strip()
    body = self._clean_blocks(body)
    output: list[str] = []
    if self.title:
      output.extend([f"# {html.escape(self.title, quote=False)}", ""])
    output.append(body)

    if self.footnotes:
      output.extend(["", "## Notes", ""])
      for number, footnote in enumerate(self.footnotes, start=1):
        output.append(f"[^{number}]: {self._one_line(footnote)}")

    return "\n".join(output).rstrip() + "\n"

  def warnings(self) -> list[str]:
    messages = []
    if self.unknown_commands:
      messages.append(
          "unrecognized commands rendered transparently: "
          + ", ".join(f"@{name}" for name in sorted(self.unknown_commands))
      )
    if self.unknown_environments:
      messages.append(
          "unrecognized environments rendered transparently: "
          + ", ".join(sorted(self.unknown_environments))
      )
    return messages

  def _collect_macros(self, nodes: list[Node]) -> None:
    for node in nodes:
      if isinstance(node, Command) and node.name in {"string", "commandstring"}:
        if not node.argument:
          continue
        match = re.match(r"\s*([^=,\s]+)\s*=\s*(.*?)\s*$", node.argument, re.S)
        if not match:
          continue
        name, value = match.groups()
        if len(value) >= 2 and value[0] == value[-1] == '"':
          value = value[1:-1]
        self.macros[name.lower()] = Parser(value).parse()
      elif isinstance(node, Environment):
        self._collect_macros(node.children)

  def _render_nodes(self, nodes: list[Node]) -> str:
    return "".join(self._render_node(node) for node in nodes)

  def _render_node(self, node: Node) -> str:
    if isinstance(node, Text):
      return self._render_text(node.value)
    if isinstance(node, Environment):
      return self._render_environment(node)
    return self._render_command(node)

  @staticmethod
  def _render_text(text: str) -> str:
    escaped = html.escape(text, quote=False)
    for character in ("\\", "`", "*", "_"):
      escaped = escaped.replace(character, f"\\{character}")
    escaped = re.sub(r"(?<!\n)\n(?!\n)", " ", escaped)
    return re.sub(r"\n{3,}", "\n\n", escaped)

  def _argument_nodes(self, command: Command) -> list[Node]:
    return Parser(command.argument or "").parse()

  def _inline_argument(self, command: Command) -> str:
    return self._one_line(self._render_nodes(self._argument_nodes(command)))

  def _plain_argument(self, command: Command) -> str:
    return self._one_line(self._render_plain(self._argument_nodes(command)))

  def _render_command(self, command: Command) -> str:
    name = command.name
    if name in {"string", "commandstring"} or name in self.IGNORED_COMMANDS:
      return ""
    if name == "\\":
      return "\x00DESCRIPTION\x00"
    if name in self.STRUCTURAL_COMMANDS:
      level = self.STRUCTURAL_COMMANDS[name]
      title = self._inline_argument(command)
      return f"\n\n{'#' * level} {title}\n\n"
    if name in {"b"}:
      return f"<strong>{self._inline_argument(command)}</strong>"
    if name in {"i"}:
      return f"<em>{self._inline_argument(command)}</em>"
    if name in {"u", "ux"}:
      return f"<u>{self._inline_argument(command)}</u>"
    if name in {"p", "example"}:
      return self._code_span(self._plain_argument(command))
    if name == "+":
      return f"<sup>{self._inline_argument(command)}</sup>"
    if name == "-":
      return f"<sub>{self._inline_argument(command)}</sub>"
    if name == "center":
      return f'\n\n<p align="center">{self._inline_argument(command)}</p>\n\n'
    if name == "caption":
      return f"\n\n**Figure:** {self._inline_argument(command)}\n\n"
    if name in {"label", "tag"}:
      label = self._plain_argument(command)
      return f'<a id="{self._slug(label)}"></a>'
    if name == "ref":
      label = self._plain_argument(command)
      return f"[{html.escape(label, quote=False)}](#{self._slug(label)})"
    if name == "cite":
      return f" [{self._inline_argument(command)}]"
    if name == "foot":
      footnote = self._render_nodes(self._argument_nodes(command)).strip()
      self.footnotes.append(footnote)
      return f"[^{len(self.footnotes)}]"
    if name == "value":
      key = self._plain_argument(command).lower()
      if key in self.macros:
        return self._render_nodes(self.macros[key])
      if key == "filedate":
        return html.escape(self.file_date or "unknown date", quote=False)
      if key == "page":
        return ""
      return html.escape(key, quote=False)
    if name in self.macros:
      return self._render_nodes(self.macros[name])

    if self.strict:
      raise ScribeError(f"unsupported Scribe command: @{name}")
    self.unknown_commands.add(name)
    if command.argument is not None:
      return self._inline_argument(command)
    return f"@{name}"

  def _render_environment(self, environment: Environment) -> str:
    name = environment.name
    if name == "example" or name == "format":
      content = self._render_plain(environment.children).strip("\n")
      return f"\n\n```text\n{content}\n```\n\n"
    if name == "enumerate":
      return self._render_list(environment.children, ordered=True)
    if name == "itemize":
      return self._render_list(environment.children, ordered=False)
    if name == "description":
      return self._render_descriptions(environment.children)
    if name in self.TRANSPARENT_ENVIRONMENTS:
      return self._render_nodes(environment.children)

    if self.strict:
      raise ScribeError(f"unsupported Scribe environment: {name}")
    self.unknown_environments.add(name)
    return self._render_nodes(environment.children)

  def _render_list(self, nodes: list[Node], *, ordered: bool) -> str:
    rendered_items = []
    for group in self._paragraph_groups(nodes):
      item = self._clean_blocks(self._render_nodes(group)).strip()
      if not item:
        continue
      marker = "1." if ordered else "-"
      rendered_items.append(self._indent_item(marker, item))
    return "\n\n" + "\n".join(rendered_items) + "\n\n"

  def _render_descriptions(self, nodes: list[Node]) -> str:
    rendered_items = []
    for group in self._paragraph_groups(nodes):
      item = self._render_nodes(group)
      item = re.sub(r"[ \t]+\n", "\n", item)
      item = re.sub(r"\n{3,}", "\n\n", item).strip()
      if not item:
        continue
      if "\x00DESCRIPTION\x00" not in item:
        if rendered_items:
          rendered_items[-1] += "\n" + self._indent(item, 2)
        else:
          rendered_items.append(item)
        continue
      term, description = item.split("\x00DESCRIPTION\x00", maxsplit=1)
      term = term.strip()
      if term.startswith("<strong>") and term.endswith("</strong>"):
        term = term[len("<strong>"):-len("</strong>")]
      rendered_items.append(
          self._indent_item("-", f"<strong>{term}</strong> — {description.strip()}")
      )
    return "\n\n" + "\n".join(rendered_items) + "\n\n"

  def _render_plain(self, nodes: list[Node]) -> str:
    output = []
    for node in nodes:
      if isinstance(node, Text):
        output.append(node.value)
      elif isinstance(node, Environment):
        output.append(self._render_plain(node.children))
      elif node.name == "\\":
        output.append("\t")
      elif node.name in self.IGNORED_COMMANDS or node.name in {
          "string",
          "commandstring",
          "label",
          "tag",
      }:
        continue
      elif node.name == "foot":
        output.append(f" ({self._plain_argument(node)})")
      elif node.name == "ref" or node.name == "cite":
        output.append(self._plain_argument(node))
      elif node.name == "value":
        key = self._plain_argument(node).lower()
        if key in self.macros:
          output.append(self._render_plain(self.macros[key]))
        elif key == "filedate":
          output.append(self.file_date or "unknown date")
      elif node.name in self.macros:
        output.append(self._render_plain(self.macros[node.name]))
      elif node.argument is not None:
        output.append(self._render_plain(self._argument_nodes(node)))
      elif node.name not in {"cbon", "cboff", "bar", "hinge"}:
        output.append(f"@{node.name}")
    return "".join(output)

  @staticmethod
  def _paragraph_groups(nodes: list[Node]) -> list[list[Node]]:
    groups: list[list[Node]] = []
    current: list[Node] = []

    for node in nodes:
      if not isinstance(node, Text):
        if isinstance(node, Environment) and not current and groups:
          groups[-1].append(node)
        else:
          current.append(node)
        continue

      pieces = re.split(r"(\n[ \t]*\n+)", node.value)
      for piece in pieces:
        if re.fullmatch(r"\n[ \t]*\n+", piece or ""):
          if current:
            groups.append(current)
            current = []
        elif piece:
          current.append(Text(piece))

    if current:
      groups.append(current)
    return groups

  @staticmethod
  def _code_span(text: str) -> str:
    longest = max((len(run) for run in re.findall(r"`+", text)), default=0)
    fence = "`" * (longest + 1)
    padding = " " if text.startswith("`") or text.endswith("`") else ""
    return f"{fence}{padding}{text}{padding}{fence}"

  @staticmethod
  def _slug(label: str) -> str:
    slug = re.sub(r"[^a-z0-9]+", "-", label.lower()).strip("-")
    return slug or "reference"

  @staticmethod
  def _one_line(text: str) -> str:
    return re.sub(r"\s+", " ", text).strip()

  @staticmethod
  def _indent(text: str, spaces: int) -> str:
    prefix = " " * spaces
    return "\n".join(prefix + line if line else prefix for line in text.splitlines())

  @classmethod
  def _indent_item(cls, marker: str, text: str) -> str:
    lines = text.splitlines() or [""]
    continuation = " " * (len(marker) + 1)
    return f"{marker} {lines[0]}" + "".join(
        f"\n{continuation}{line}" for line in lines[1:]
    )

  @staticmethod
  def _clean_blocks(text: str) -> str:
    text = text.replace("\x00DESCRIPTION\x00", " — ")
    text = re.sub(r"[ \t]+\n", "\n", text)
    text = re.sub(r"\n{3,}", "\n\n", text)
    text = re.sub(r"(?m)^ (?=\S)", "", text)
    return text


def convert_scribe(
    source: str,
    *,
    strict: bool = False,
    file_date: str | None = None,
    title: str | None = None,
) -> tuple[str, list[str]]:
  """Convert Scribe source and return Markdown plus non-fatal warnings."""
  nodes = Parser(source.removeprefix("\ufeff")).parse()
  renderer = Renderer(strict=strict, file_date=file_date, title=title)
  markdown = renderer.render(nodes)
  return markdown, renderer.warnings()


def _parser() -> argparse.ArgumentParser:
  parser = argparse.ArgumentParser(
      description="Convert a legacy Scribe .mss manuscript to GitHub Markdown."
  )
  parser.add_argument("input", help="input .mss file, or - for standard input")
  parser.add_argument(
      "output",
      nargs="?",
      help="output .md file (default: input name with an .md suffix)",
  )
  parser.add_argument("-o", "--output", dest="output_option", metavar="FILE")
  parser.add_argument("--encoding", default="utf-8", help="input encoding")
  parser.add_argument("--title", help="prepend this level-one heading")
  parser.add_argument(
      "--file-date",
      help="replacement for Scribe's dynamic @value(filedate)",
  )
  parser.add_argument(
      "--strict",
      action="store_true",
      help="fail instead of transparently rendering unknown constructs",
  )
  return parser


def main(argv: Sequence[str] | None = None) -> int:
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
      source = sys.stdin.buffer.read().decode(args.encoding)
    except (LookupError, UnicodeDecodeError) as error:
      parser.error(str(error))
  else:
    input_path = Path(args.input)
    if input_path.suffix.lower() != ".mss":
      parser.error(f"input file must have an .mss extension: {input_path}")
    try:
      source = input_path.read_text(encoding=args.encoding)
    except (OSError, LookupError, UnicodeError) as error:
      parser.error(f"cannot read {input_path}: {error}")
    if args.output is None:
      args.output = str(input_path.with_suffix(".md"))

  try:
    markdown, warnings = convert_scribe(
        source,
        strict=args.strict,
        file_date=args.file_date,
        title=args.title,
    )
  except ScribeError as error:
    parser.error(str(error))

  for warning in warnings:
    print(f"warning: {warning}", file=sys.stderr)

  if args.output == "-":
    sys.stdout.write(markdown)
    return 0

  output_path = Path(args.output)
  if output_path.suffix.lower() not in {".md", ".markdown"}:
    parser.error(f"output file must have an .md or .markdown extension: {output_path}")
  if args.input != "-" and output_path.resolve() == Path(args.input).resolve():
    parser.error("input and output paths must be different")
  try:
    output_path.write_text(markdown, encoding="utf-8", newline="\n")
  except OSError as error:
    parser.error(f"cannot write {output_path}: {error}")
  return 0


if __name__ == "__main__":
  raise SystemExit(main())
