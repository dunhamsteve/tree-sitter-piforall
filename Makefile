
default: tree-sitter-piforall.wasm

src/grammar.json: grammar.js
	tree-sitter generate

tree-sitter-piforall.wasm: src/grammar.json src/scanner.c
	tree-sitter build --wasm

playground: tree-sitter-piforall.wasm
	tree-sitter playground

test: src/grammar.json
	tree-sitter test

clean:
	rm -rf src/tree_sitter src/*.json src/parser.c bindings *.wasm


