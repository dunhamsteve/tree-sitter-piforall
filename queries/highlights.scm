
[
    "if" "then" "else"
] @keyword ; conditional

[
"let" "in"
"subst" "by"
"contra"
"Refl"
"\\" "." "=" "->" ":" "*"
] @keyword
[
  "("
  ")"
  "{" "|" "}"
  ; "["
] @keyword ; @punctuation.bracket
(comment) @comment
(valDef) @local.scope
(letPairExp
    (variable (identifier)  @local.definition)
    ) @local.scope


(variable) @local.reference
