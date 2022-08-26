" Vim syntax file
" Language:     Roast
" Maintainer:   Patrick Walton <pcwalton@mozilla.com>
" Maintainer:   Ben Blum <bblum@cs.cmu.edu>
" Maintainer:   Chris Morgan <me@chrismorgan.info>
" Last Change:  Feb 24, 2016

if version < 600
  syntax clear
elseif exists("b:current_syntax")
  finish
endif

" Syntax definitions {{{1
" Basic keywords {{{2
syn keyword   cramConditional match if else
syn keyword   cramRepeat for loop while
syn keyword   cramTypedef type nextgroup=cramIdentifier skipwhite skipempty
syn keyword   cramStructure struct class enum nextgroup=cramIdentifier skipwhite skipempty
syn keyword   cramUnion union nextgroup=cramIdentifier skipwhite skipempty contained
syn match cramUnionContextual /\<union\_s\+\%([^[:cntrl:][:space:][:punct:][:digit:]]\|_\)\%([^[:cntrl:][:punct:][:space:]]\|_\)*/ transparent contains=cramUnion
syn keyword   cramOperator    as is

syn match     cramAssert      "\<assert\(\w\)*!" contained
syn match     cramPanic       "\<panic\(\w\)*!" contained
syn keyword   cramKeyword     break
syn keyword   cramKeyword     box nextgroup=cramBoxPlacement skipwhite skipempty
syn keyword   cramKeyword     continue
syn keyword   cramKeyword     extern nextgroup=cramExternCrate,cramObsoleteExternMod skipwhite skipempty
syn keyword   cramKeyword     fun nextgroup=cramFuncName skipwhite skipempty
syn keyword   cramKeyword     in extension let var new cram pull loc this single 
syn keyword   cramKeyword     public private final import using  nextgroup=cramPubScope skipwhite skipempty
syn keyword   cramKeyword     return
syn keyword   cramSuper       super
syn keyword   cramKeyword     safe where free
syn keyword   cramKeyword     use nextgroup=cramModPath skipwhite skipempty
" FIXME: Scoped impl's name is also fallen in this category
syn keyword   cramKeyword     module interface nextgroup=cramIdentifier skipwhite skipempty
syn keyword   cramStorage     give own ref static const
syn match cramDefault /\<default\ze\_s\+\(impl\|fn\|type\|const\)\>/

syn keyword   cramInvalidBareKeyword crate  

syn keyword cramPubScopeCrate crate contained
syn match cramPubScopeDelim /[()]/ contained
syn match cramPubScope /([^()]*)/ contained contains=cramPubScopeDelim,cramPubScopeCrate,cramSuper,cramModPath,cramModPathSep,cramSelf transparent

syn keyword   cramExternCrate crate contained nextgroup=cramIdentifier,cramExternCrateString skipwhite skipempty
" This is to get the `bar` part of `extern crate "foo" as bar;` highlighting.
syn match   cramExternCrateString /".*"\_s*as/ contained nextgroup=cramIdentifier skipwhite transparent skipempty contains=cramString,cramOperator
syn keyword   cramObsoleteExternMod mod contained nextgroup=cramIdentifier skipwhite skipempty

syn match     cramIdentifier  contains=cramIdentifierPrime "\%([^[:cntrl:][:space:][:punct:][:digit:]]\|_\)\%([^[:cntrl:][:punct:][:space:]]\|_\)*" display contained
syn match     cramFuncName    "\%([^[:cntrl:][:space:][:punct:][:digit:]]\|_\)\%([^[:cntrl:][:punct:][:space:]]\|_\)*" display contained

syn region    cramBoxPlacement matchgroup=cramBoxPlacementParens start="(" end=")" contains=TOP contained
" Ideally we'd have syntax rules set up to match arbitrary expressions. Since
" we don't, we'll just define temporary contained rules to handle balancing
" delimiters.
syn region    cramBoxPlacementBalance start="(" end=")" containedin=cramBoxPlacement transparent
syn region    cramBoxPlacementBalance start="\[" end="\]" containedin=cramBoxPlacement transparent
" {} are handled by cramFoldBraces

syn region cramMacroRepeat matchgroup=cramMacroRepeatDelimiters start="$(" end=")" contains=TOP nextgroup=cramMacroRepeatCount
syn match cramMacroRepeatCount ".\?[*+]" contained
syn match cramMacroVariable "$\w\+"

" Reserved (but not yet used) keywords {{{2
syn keyword   cramReservedKeyword alignof become do offsetof  pure sizeof typeof unsized yield abstract virtural override macro

" Built-in types {{{2
syn keyword   cramType        isize usize char bool u8 u16 u32 u64 f32
syn keyword   cramType        f64 i8 i16 i32 i64 string Self

" Things from the libstd v1 prelude (src/libstd/prelude/v1.rs) {{{2
" This section is just straight transformation of the contents of the prelude,
" to make it easy to update.

" Reexported core operators {{{3
syn keyword   cramTrait       Copy Send Sized Sync
syn keyword   cramTrait       Drop Fn FnMut FnOnce

" Reexported functions {{{3
" There’s no point in highlighting these; when one writes drop( or drop::< it
" gets the same highlighting anyway, and if someone writes `let drop = …;` we
" don’t really want *that* drop to be highlighted.
"syn keyword cramFunction drop

" Reexported types and traits {{{3
syn keyword cramTrait Box
syn keyword cramTrait ToOwned
syn keyword cramTrait Clone
syn keyword cramTrait PartialEq PartialOrd Eq Ord
syn keyword cramTrait AsRef AsMut Into From
syn keyword cramTrait Default
syn keyword cramTrait Iterator Extend IntoIterator
syn keyword cramTrait DoubleEndedIterator ExactSizeIterator
syn keyword cramEnum Option
syn keyword cramEnumVariant Some None
syn keyword cramEnum Result
syn keyword cramEnumVariant Ok Err
syn keyword cramTrait SliceConcatExt
syn keyword cramTrait String ToString
syn keyword cramTrait Vec

" Other syntax {{{2
syn keyword   cramSelf        self
syn keyword   cramBoolean     true false

" If foo::bar changes to foo.bar, change this ("::" to "\.").
" If foo::bar changes to Foo::bar, change this (first "\w" to "\u").
syn match     cramModPath     "\w\(\w\)*::[^<]"he=e-3,me=e-3
syn match     cramModPathSep  "::"

syn match     cramFuncCall    "\w\(\w\)*("he=e-1,me=e-1
syn match     cramFuncCall    "\w\(\w\)*::<"he=e-3,me=e-3 " foo::<T>();

" This is merely a convention; note also the use of [A-Z], restricting it to
" latin identifiers rather than the full Unicode uppercase. I have not used
" [:upper:] as it depends upon 'noignorecase'
"syn match     cramCapsIdent    display "[A-Z]\w\(\w\)*"

syn match     cramOperator     display "\%(+\|-\|/\|*\|=\|\^\|&\||\|!\|>\|<\|%\)=\?"
" This one isn't *quite* right, as we could have binary-& with a reference
syn match     cramSigil        display /&\s\+[&~@*][^)= \t\r\n]/he=e-1,me=e-1
syn match     cramSigil        display /[&~@*][^)= \t\r\n]/he=e-1,me=e-1
" This isn't actually correct; a closure with no arguments can be `|| { }`.
" Last, because the & in && isn't a sigil
syn match     cramOperator     display "&&\|||"
" This is cramArrowCharacter rather than cramArrow for the sake of matchparen,
" so it skips the ->; see http://stackoverflow.com/a/30309949 for details.
syn match     cramArrowCharacter display "->"

syn match     cramMacro       '\w\(\w\)*!' contains=cramAssert,cramPanic
syn match     cramMacro       '#\w\(\w\)*' contains=cramAssert,cramPanic

syn match     cramEscapeError   display contained /\\./
syn match     cramEscape        display contained /\\\([nrt0\\'"]\|x\x\{2}\)/
syn match     cramEscapeUnicode display contained /\\u{\x\{1,6}}/
syn match     cramStringContinuation display contained /\\\n\s*/
syn region    cramString      start=+b"+ skip=+\\\\\|\\"+ end=+"+ contains=cramEscape,cramEscapeError,cramStringContinuation
syn region    cramString      start=+"+ skip=+\\\\\|\\"+ end=+"+ contains=cramEscape,cramEscapeUnicode,cramEscapeError,cramStringContinuation,@Spell
syn region    cramString      start='b\?r\z(#*\)"' end='"\z1' contains=@Spell

syn region    cramAttribute   start="#!\?\[" end="\]" contains=cramString,cramDerive,cramCommentLine,cramCommentBlock,cramCommentLineDocError,cramCommentBlockDocError
syn region    cramDerive      start="derive(" end=")" contained contains=cramDeriveTrait
" This list comes from src/libsyntax/ext/deriving/mod.rs
" Some are deprecated (Encodable, Decodable) or to be removed after a new snapshot (Show).
syn keyword   cramDeriveTrait contained Clone Hash RoastcEncodable RoastcDecodable Encodable Decodable PartialEq Eq PartialOrd Ord Rand Show Debug Default FromPrimitive Send Sync Copy

" Number literals
syn match     cramDecNumber   display "\<[0-9][0-9_]*\%([iu]\%(size\|8\|16\|32\|64\)\)\="
syn match     cramHexNumber   display "\<0x[a-fA-F0-9_]\+\%([iu]\%(size\|8\|16\|32\|64\)\)\="
syn match     cramOctNumber   display "\<0o[0-7_]\+\%([iu]\%(size\|8\|16\|32\|64\)\)\="
syn match     cramBinNumber   display "\<0b[01_]\+\%([iu]\%(size\|8\|16\|32\|64\)\)\="

" Special case for numbers of the form "1." which are float literals, unless followed by
" an identifier, which makes them integer literals with a method call or field access,
" or by another ".", which makes them integer literals followed by the ".." token.
" (This must go first so the others take precedence.)
syn match     cramFloat       display "\<[0-9][0-9_]*\.\%([^[:cntrl:][:space:][:punct:][:digit:]]\|_\|\.\)\@!"
" To mark a number as a normal float, it must have at least one of the three things integral values don't have:
" a decimal point and more numbers; an exponent; and a type suffix.
syn match     cramFloat       display "\<[0-9][0-9_]*\%(\.[0-9][0-9_]*\)\%([eE][+-]\=[0-9_]\+\)\=\(f32\|f64\)\="
syn match     cramFloat       display "\<[0-9][0-9_]*\%(\.[0-9][0-9_]*\)\=\%([eE][+-]\=[0-9_]\+\)\(f32\|f64\)\="
syn match     cramFloat       display "\<[0-9][0-9_]*\%(\.[0-9][0-9_]*\)\=\%([eE][+-]\=[0-9_]\+\)\=\(f32\|f64\)"

" For the benefit of delimitMate
syn region cramLifetimeCandidate display start=/&'\%(\([^'\\]\|\\\(['nrt0\\\"]\|x\x\{2}\|u{\x\{1,6}}\)\)'\)\@!/ end=/[[:cntrl:][:space:][:punct:]]\@=\|$/ contains=cramSigil,cramLifetime
syn region cramGenericRegion display start=/<\%('\|[^[cntrl:][:space:][:punct:]]\)\@=')\S\@=/ end=/>/ contains=cramGenericLifetimeCandidate
syn region cramGenericLifetimeCandidate display start=/\%(<\|,\s*\)\@<='/ end=/[[:cntrl:][:space:][:punct:]]\@=\|$/ contains=cramSigil,cramLifetime

"cramLifetime must appear before cramCharacter, or chars will get the lifetime highlighting
syn match     cramLifetime    display "\'\%([^[:cntrl:][:space:][:punct:][:digit:]]\|_\)\%([^[:cntrl:][:punct:][:space:]]\|_\)*"
syn match     cramLabel       display "\'\%([^[:cntrl:][:space:][:punct:][:digit:]]\|_\)\%([^[:cntrl:][:punct:][:space:]]\|_\)*:"
syn match   cramCharacterInvalid   display contained /b\?'\zs[\n\r\t']\ze'/
" The groups negated here add up to 0-255 but nothing else (they do not seem to go beyond ASCII).
syn match   cramCharacterInvalidUnicode   display contained /b'\zs[^[:cntrl:][:graph:][:alnum:][:space:]]\ze'/
syn match   cramCharacter   /b'\([^\\]\|\\\(.\|x\x\{2}\)\)'/ contains=cramEscape,cramEscapeError,cramCharacterInvalid,cramCharacterInvalidUnicode
syn match   cramCharacter   /'\([^\\]\|\\\(.\|x\x\{2}\|u{\x\{1,6}}\)\)'/ contains=cramEscape,cramEscapeUnicode,cramEscapeError,cramCharacterInvalid

syn match cramShebang /\%^#![^[].*/
syn region cramCommentLine                                                  start="//"                      end="$"   contains=cramTodo,@Spell
syn region cramCommentLineDoc                                               start="//\%(//\@!\|!\)"         end="$"   contains=cramTodo,@Spell
syn region cramCommentLineDocError                                          start="//\%(//\@!\|!\)"         end="$"   contains=cramTodo,@Spell contained
syn region cramCommentBlock             matchgroup=cramCommentBlock         start="/\*\%(!\|\*[*/]\@!\)\@!" end="\*/" contains=cramTodo,cramCommentBlockNest,@Spell
syn region cramCommentBlockDoc          matchgroup=cramCommentBlockDoc      start="/\*\%(!\|\*[*/]\@!\)"    end="\*/" contains=cramTodo,cramCommentBlockDocNest,@Spell
syn region cramCommentBlockDocError     matchgroup=cramCommentBlockDocError start="/\*\%(!\|\*[*/]\@!\)"    end="\*/" contains=cramTodo,cramCommentBlockDocNestError,@Spell contained
syn region cramCommentBlockNest         matchgroup=cramCommentBlock         start="/\*"                     end="\*/" contains=cramTodo,cramCommentBlockNest,@Spell contained transparent
syn region cramCommentBlockDocNest      matchgroup=cramCommentBlockDoc      start="/\*"                     end="\*/" contains=cramTodo,cramCommentBlockDocNest,@Spell contained transparent
syn region cramCommentBlockDocNestError matchgroup=cramCommentBlockDocError start="/\*"                     end="\*/" contains=cramTodo,cramCommentBlockDocNestError,@Spell contained transparent
" FIXME: this is a really ugly and not fully correct implementation. Most
" importantly, a case like ``/* */*`` should have the final ``*`` not being in
" a comment, but in practice at present it leaves comments open two levels
" deep. But as long as you stay away from that particular case, I *believe*
" the highlighting is correct. Due to the way Vim's syntax engine works
" (greedy for start matches, unlike Roast's tokeniser which is searching for
" the earliest-starting match, start or end), I believe this cannot be solved.
" Oh you who would fix it, don't bother with things like duplicating the Block
" rules and putting ``\*\@<!`` at the start of them; it makes it worse, as
" then you must deal with cases like ``/*/**/*/``. And don't try making it
" worse with ``\%(/\@<!\*\)\@<!``, either...

syn keyword cramTodo contained TODO FIXME XXX NB NOTE

" Folding rules {{{2
" Trivial folding rules to begin with.
" FIXME: use the AST to make really good folding
syn region cramFoldBraces start="{" end="}" transparent fold

" Default highlighting {{{1
hi def link cramDecNumber       cramNumber
hi def link cramHexNumber       cramNumber
hi def link cramOctNumber       cramNumber
hi def link cramBinNumber       cramNumber
hi def link cramIdentifierPrime cramIdentifier
hi def link cramTrait           cramType
hi def link cramDeriveTrait     cramTrait

hi def link cramMacroRepeatCount   cramMacroRepeatDelimiters
hi def link cramMacroRepeatDelimiters   Macro
hi def link cramMacroVariable Define
hi def link cramSigil         StorageClass
hi def link cramEscape        Special
hi def link cramEscapeUnicode cramEscape
hi def link cramEscapeError   Error
hi def link cramStringContinuation Special
hi def link cramString        String
hi def link cramCharacterInvalid Error
hi def link cramCharacterInvalidUnicode cramCharacterInvalid
hi def link cramCharacter     Character
hi def link cramNumber        Number
hi def link cramBoolean       Boolean
hi def link cramEnum          cramType
hi def link cramEnumVariant   cramConstant
hi def link cramConstant      Constant
hi def link cramSelf          Constant
hi def link cramFloat         Float
hi def link cramArrowCharacter cramOperator
hi def link cramOperator      Operator
hi def link cramKeyword       Keyword
hi def link cramTypedef       Keyword " More precise is Typedef, but it doesn't feel right for Roast
hi def link cramStructure     Keyword " More precise is Structure
hi def link cramUnion         cramStructure
hi def link cramPubScopeDelim Delimiter
hi def link cramPubScopeCrate cramKeyword
hi def link cramSuper         cramKeyword
hi def link cramReservedKeyword Error
hi def link cramRepeat        Conditional
hi def link cramConditional   Conditional
hi def link cramIdentifier    Identifier
hi def link cramCapsIdent     cramIdentifier
hi def link cramModPath       Include
hi def link cramModPathSep    Delimiter
hi def link cramFunction      Function
hi def link cramFuncName      Function
hi def link cramFuncCall      Function
hi def link cramShebang       Comment
hi def link cramCommentLine   Comment
hi def link cramCommentLineDoc SpecialComment
hi def link cramCommentLineDocError Error
hi def link cramCommentBlock  cramCommentLine
hi def link cramCommentBlockDoc cramCommentLineDoc
hi def link cramCommentBlockDocError Error
hi def link cramAssert        PreCondit
hi def link cramPanic         PreCondit
hi def link cramMacro         Macro
hi def link cramType          Type
hi def link cramTodo          Todo
hi def link cramAttribute     PreProc
hi def link cramDerive        PreProc
hi def link cramDefault       StorageClass
hi def link cramStorage       StorageClass
hi def link cramObsoleteStorage Error
hi def link cramLifetime      Special
hi def link cramLabel         Label
hi def link cramInvalidBareKeyword Error
hi def link cramExternCrate   cramKeyword
hi def link cramObsoleteExternMod Error
hi def link cramBoxPlacementParens Delimiter

" Other Suggestions:
" hi cramAttribute ctermfg=cyan
" hi cramDerive ctermfg=cyan
" hi cramAssert ctermfg=yellow
" hi cramPanic ctermfg=red
" hi cramMacro ctermfg=magenta

syn sync minlines=200
syn sync maxlines=500

let b:current_syntax = "cram"
