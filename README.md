# tree-sitter grammar for pi-forall

This was a tree-sitter parser for pi-forall that I made a couple of years ago. It was an experiment to learn how to do off-side (indentation sensitive) parsing in tree-sitter. I've recently updated it to fix some issues and cover more of the language.

`make playground` to bring up the playground UI.

`sample.pi` is test case from the piforall repository (`Lec4.pi`) and `sample.query` is a sample "query" for syntax highlighting.  I haven't made proper tree-sitter test cases yet.

## off-side parsing

I solved the off-side (indentation sensitive) parsing problem with a custom scanner. For layout blocks, it provides tokens for a virtual start, semicolon, and end. It also handles whitespace. Tree-sitter is kept from seeing outside the indentation level by refusing to skip whitspace if it doesn't end at an acceptable column.

