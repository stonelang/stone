" Vim syntax file
" Language:     Rust
" Maintainer:   Patrick Walton <pcwalton@mozilla.com>
" Maintainer:   Ben Blum <bblum@cs.cmu.edu>
" Maintainer:   Chris Morgan <me@chrismorgan.info>
" Last Change:  Feb 24, 2016
" For bugs, patches and license go to https://github.com/go-lang/go.vim

if version < 600
	syntax clear
elseif exists("b:current_syntax")
	finish
endif

" Syntax definitions {{{1
" Basic keywords {{{2
syn keyword   goConditional match if else switch
syn keyword   goRepeat for loop while do
syn keyword   goTypedef type nextgroup=goIdentifier skipwhite skipempty
syn keyword   goStructure struct enum nextgroup=goIdentifier skipwhite skipempty
syn keyword   goUnion union nextgroup=goIdentifier skipwhite skipempty contained
syn match goUnionContextual /\<union\_s\+\%([^[:cntrl:][:space:][:punct:][:digit:]]\|_\)\%([^[:cntrl:][:punct:][:space:]]\|_\)*/ transparent contains=goUnion
syn keyword   goOperator    as

syn match     goAssert      "\<assert\(\w\)*!" contained
syn match     goPanic       "\<panic\(\w\)*!" contained
syn keyword   goKeyword     break pure type byte rune uint8 uint64 virtual override package interface where generic class extension  
syn keyword   goKeyword     box nextgroup=goBoxPlacement skipwhite skipempty
syn keyword   goKeyword     continue new property import free group module import with
syn keyword   goKeyword     extern nextgroup=goExternCrate,goObsoleteExternMod skipwhite skipempty
syn keyword   goKeyword     fun nextgroup=goFuncName skipwhite skipempty
syn keyword   goKeyword     func nextgroup=goFuncName skipwhite skipempty

syn keyword   goKeyword     in let map uint64 var is private protected public include safe 
syn keyword		goKeyword		 this auto friend class namespace group  
syn keyword   goKeyword     final delete extension val
syn keyword   goKeyword     public  nextgroup=goPubScope skipwhite skipempty
syn keyword   goKeyword     return void using nullptr null can 
syn keyword   goSuper       super
syn keyword   goKeyword     unsafe where safe own raw cram pull mutable inline case switch default
syn keyword   goKeyword     use nextgroup=goModPath skipwhite skipempty
" FIXME: Scoped impl's name is also fallen in this category
syn keyword   goKeyword     mod trait nextgroup=goIdentifier skipwhite skipempty
syn keyword   goStorage     move mut ref static const
syn match goDefault /\<default\ze\_s\+\(impl\|fn\|type\|const\)\>/

syn keyword   goInvalidBareKeyword crate

syn keyword goPubScopeCrate crate contained
syn match goPubScopeDelim /[()]/ contained
syn match goPubScope /([^()]*)/ contained contains=goPubScopeDelim,goPubScopeCrate,goSuper,goModPath,goModPathSep,goSelf transparent

syn keyword   goExternCrate crate contained nextgroup=goIdentifier,goExternCrateString skipwhite skipempty
" This is to get the `bar` part of `extern crate "foo" as bar;` highlighting.
syn match   goExternCrateString /".*"\_s*as/ contained nextgroup=goIdentifier skipwhite transparent skipempty contains=goString,goOperator
syn keyword   goObsoleteExternMod mod contained nextgroup=goIdentifier skipwhite skipempty

syn match     goIdentifier  contains=goIdentifierPrime "\%([^[:cntrl:][:space:][:punct:][:digit:]]\|_\)\%([^[:cntrl:][:punct:][:space:]]\|_\)*" display contained
syn match     goFuncName    "\%([^[:cntrl:][:space:][:punct:][:digit:]]\|_\)\%([^[:cntrl:][:punct:][:space:]]\|_\)*" display contained

syn region    goBoxPlacement matchgroup=goBoxPlacementParens start="(" end=")" contains=TOP contained
" Ideally we'd have syntax rules set up to match arbitrary expressions. Since
" we don't, we'll just define temporary contained rules to handle balancing
" delimiters.
syn region    goBoxPlacementBalance start="(" end=")" containedin=goBoxPlacement transparent
syn region    goBoxPlacementBalance start="\[" end="\]" containedin=goBoxPlacement transparent
" {} are handled by goFoldBraces

syn region goMacroRepeat matchgroup=goMacroRepeatDelimiters start="$(" end=")" contains=TOP nextgroup=goMacroRepeatCount
syn match goMacroRepeatCount ".\?[*+]" contained
syn match goMacroVariable "$\w\+"

" Reserved (but not yet used) keywords {{{2
syn keyword   goReservedKeyword alignof become offsetof priv sizeof typeof unsized yield abstract  macro

" Built-in types {{{2
syn keyword   goType        isize usize char bool u8 u16 u32 u64 u128 f32
syn keyword   goType        f64 i8 i16 i32 i64 i128 string this int unsigned 

" Things from the libstd v1 prelude (src/libstd/prelude/v1.rs) {{{2
" This section is just straight transformation of the contents of the prelude,
" to make it easy to update.

" Reexported core operators {{{3
syn keyword   goTrait       Copy Send Sized Sync
syn keyword   goTrait       Drop Fn FnMut FnOnce

" Reexported functions {{{3
" There’s no point in highlighting these; when one writes drop( or drop::< it
" gets the same highlighting anyway, and if someone writes `let drop = …;` we
" don’t really want *that* drop to be highlighted.
"syn keyword goFunction drop

" Reexported types and traits {{{3
"syn keyword goTrait Box
"syn keyword goTrait ToOwned
"syn keyword goTrait Clone
"syn keyword goTrait PartialEq PartialOrd Eq Ord
"syn keyword goTrait AsRef AsMut Into From
"syn keyword goTrait Default
"syn keyword goTrait Iterator Extend IntoIterator
"syn keyword goTrait DoubleEndedIterator ExactSizeIterator
"syn keyword goEnum Option
"syn keyword goEnumVariant Some None
"syn keyword goEnum Result
"syn keyword goEnumVariant Ok Err
"syn keyword goTrait SliceConcatExt
"syn keyword goTrait String ToString
"syn keyword goTrait Vec

" Other syntax {{{2
syn keyword   goSelf        self
syn keyword   goBoolean     true false

" If foo::bar changes to foo.bar, change this ("::" to "\.").
" If foo::bar changes to Foo::bar, change this (first "\w" to "\u").
syn match     goModPath     "\w\(\w\)*::[^<]"he=e-3,me=e-3
syn match     goModPathSep  "::"

syn match     goFuncCall    "\w\(\w\)*("he=e-1,me=e-1
syn match     goFuncCall    "\w\(\w\)*::<"he=e-3,me=e-3 " foo::<T>();

" This is merely a convention; note also the use of [A-Z], restricting it to
" latin identifiers rather than the full Unicode uppercase. I have not used
" [:upper:] as it depends upon 'noignorecase'
"syn match     goCapsIdent    display "[A-Z]\w\(\w\)*"

syn match     goOperator     display "\%(+\|-\|/\|*\|=\|\^\|&\||\|!\|>\|<\|%\)=\?"
" This one isn't *quite* right, as we could have binary-& with a reference
syn match     goSigil        display /&\s\+[&~@*][^)= \t\r\n]/he=e-1,me=e-1
syn match     goSigil        display /[&~@*][^)= \t\r\n]/he=e-1,me=e-1
" This isn't actually correct; a closure with no arguments can be `|| { }`.
" Last, because the & in && isn't a sigil
syn match     goOperator     display "&&\|||"
" This is goArrowCharacter rather than goArrow for the sake of matchparen,
" so it skips the ->; see http://stackoverflow.com/a/30309949 for details.
syn match     goArrowCharacter display "->"
syn match     goQuestionMark display "?\([a-zA-Z]\+\)\@!"

syn match     goMacro       '\w\(\w\)*!' contains=goAssert,goPanic
syn match     goMacro       '#\w\(\w\)*' contains=goAssert,goPanic

syn match     goEscapeError   display contained /\\./
syn match     goEscape        display contained /\\\([nrt0\\'"]\|x\x\{2}\)/
syn match     goEscapeUnicode display contained /\\u{\x\{1,6}}/
syn match     goStringContinuation display contained /\\\n\s*/
syn region    goString      start=+b"+ skip=+\\\\\|\\"+ end=+"+ contains=goEscape,goEscapeError,goStringContinuation
syn region    goString      start=+"+ skip=+\\\\\|\\"+ end=+"+ contains=goEscape,goEscapeUnicode,goEscapeError,goStringContinuation,@Spell
syn region    goString      start='b\?r\z(#*\)"' end='"\z1' contains=@Spell

syn region    goAttribute   start="#!\?\[" end="\]" contains=goString,goDerive,goCommentLine,goCommentBlock,goCommentLineDocError,goCommentBlockDocError
syn region    goDerive      start="derive(" end=")" contained contains=goDeriveTrait
" This list comes from src/libsyntax/ext/deriving/mod.rs
" Some are deprecated (Encodable, Decodable) or to be removed after a new snapshot (Show).
syn keyword   goDeriveTrait contained Clone Hash RustcEncodable RustcDecodable Encodable Decodable PartialEq Eq PartialOrd Ord Rand Show Debug Default FromPrimitive Send Sync Copy

" Number literals
syn match     goDecNumber   display "\<[0-9][0-9_]*\%([iu]\%(size\|8\|16\|32\|64\|128\)\)\="
syn match     goHexNumber   display "\<0x[a-fA-F0-9_]\+\%([iu]\%(size\|8\|16\|32\|64\|128\)\)\="
syn match     goOctNumber   display "\<0o[0-7_]\+\%([iu]\%(size\|8\|16\|32\|64\|128\)\)\="
syn match     goBinNumber   display "\<0b[01_]\+\%([iu]\%(size\|8\|16\|32\|64\|128\)\)\="

" Special case for numbers of the form "1." which are float literals, unless followed by
" an identifier, which makes them integer literals with a method call or field access,
" or by another ".", which makes them integer literals followed by the ".." token.
" (This must go first so the others take precedence.)
syn match     goFloat       display "\<[0-9][0-9_]*\.\%([^[:cntrl:][:space:][:punct:][:digit:]]\|_\|\.\)\@!"
" To mark a number as a normal float, it must have at least one of the three things integral values don't have:
" a decimal point and more numbers; an exponent; and a type suffix.
syn match     goFloat       display "\<[0-9][0-9_]*\%(\.[0-9][0-9_]*\)\%([eE][+-]\=[0-9_]\+\)\=\(f32\|f64\)\="
syn match     goFloat       display "\<[0-9][0-9_]*\%(\.[0-9][0-9_]*\)\=\%([eE][+-]\=[0-9_]\+\)\(f32\|f64\)\="
syn match     goFloat       display "\<[0-9][0-9_]*\%(\.[0-9][0-9_]*\)\=\%([eE][+-]\=[0-9_]\+\)\=\(f32\|f64\)"

" For the benefit of delimitMate
syn region goLifetimeCandidate display start=/&'\%(\([^'\\]\|\\\(['nrt0\\\"]\|x\x\{2}\|u{\x\{1,6}}\)\)'\)\@!/ end=/[[:cntrl:][:space:][:punct:]]\@=\|$/ contains=goSigil,goLifetime
syn region goGenericRegion display start=/<\%('\|[^[cntrl:][:space:][:punct:]]\)\@=')\S\@=/ end=/>/ contains=goGenericLifetimeCandidate
syn region goGenericLifetimeCandidate display start=/\%(<\|,\s*\)\@<='/ end=/[[:cntrl:][:space:][:punct:]]\@=\|$/ contains=goSigil,goLifetime

"goLifetime must appear before goCharacter, or chars will get the lifetime highlighting
syn match     goLifetime    display "\'\%([^[:cntrl:][:space:][:punct:][:digit:]]\|_\)\%([^[:cntrl:][:punct:][:space:]]\|_\)*"
syn match     goLabel       display "\'\%([^[:cntrl:][:space:][:punct:][:digit:]]\|_\)\%([^[:cntrl:][:punct:][:space:]]\|_\)*:"
syn match   goCharacterInvalid   display contained /b\?'\zs[\n\r\t']\ze'/
" The groups negated here add up to 0-255 but nothing else (they do not seem to go beyond ASCII).
syn match   goCharacterInvalidUnicode   display contained /b'\zs[^[:cntrl:][:graph:][:alnum:][:space:]]\ze'/
syn match   goCharacter   /b'\([^\\]\|\\\(.\|x\x\{2}\)\)'/ contains=goEscape,goEscapeError,goCharacterInvalid,goCharacterInvalidUnicode
syn match   goCharacter   /'\([^\\]\|\\\(.\|x\x\{2}\|u{\x\{1,6}}\)\)'/ contains=goEscape,goEscapeUnicode,goEscapeError,goCharacterInvalid

syn match goShebang /\%^#![^[].*/
syn region goCommentLine                                                  start="//"                      end="$"   contains=goTodo,@Spell
syn region goCommentLineDoc                                               start="//\%(//\@!\|!\)"         end="$"   contains=goTodo,@Spell
syn region goCommentLineDocError                                          start="//\%(//\@!\|!\)"         end="$"   contains=goTodo,@Spell contained
syn region goCommentBlock             matchgroup=goCommentBlock         start="/\*\%(!\|\*[*/]\@!\)\@!" end="\*/" contains=goTodo,goCommentBlockNest,@Spell
syn region goCommentBlockDoc          matchgroup=goCommentBlockDoc      start="/\*\%(!\|\*[*/]\@!\)"    end="\*/" contains=goTodo,goCommentBlockDocNest,@Spell
syn region goCommentBlockDocError     matchgroup=goCommentBlockDocError start="/\*\%(!\|\*[*/]\@!\)"    end="\*/" contains=goTodo,goCommentBlockDocNestError,@Spell contained
syn region goCommentBlockNest         matchgroup=goCommentBlock         start="/\*"                     end="\*/" contains=goTodo,goCommentBlockNest,@Spell contained transparent
syn region goCommentBlockDocNest      matchgroup=goCommentBlockDoc      start="/\*"                     end="\*/" contains=goTodo,goCommentBlockDocNest,@Spell contained transparent
syn region goCommentBlockDocNestError matchgroup=goCommentBlockDocError start="/\*"                     end="\*/" contains=goTodo,goCommentBlockDocNestError,@Spell contained transparent
" FIXME: this is a really ugly and not fully correct implementation. Most
" importantly, a case like ``/* */*`` should have the final ``*`` not being in
" a comment, but in practice at present it leaves comments open two levels
" deep. But as long as you stay away from that particular case, I *believe*
" the highlighting is correct. Due to the way Vim's syntax engine works
" (greedy for start matches, unlike Rust's tokeniser which is searching for
" the earliest-starting match, start or end), I believe this cannot be solved.
" Oh you who would fix it, don't bother with things like duplicating the Block
" rules and putting ``\*\@<!`` at the start of them; it makes it worse, as
" then you must deal with cases like ``/*/**/*/``. And don't try making it
" worse with ``\%(/\@<!\*\)\@<!``, either...

syn keyword goTodo contained TODO FIXME XXX NB NOTE

" Folding rules {{{2
" Trivial folding rules to begin with.
" FIXME: use the AST to make really good folding
syn region goFoldBraces start="{" end="}" transparent fold

" Default highlighting {{{1
hi def link goDecNumber       goNumber
hi def link goHexNumber       goNumber
hi def link goOctNumber       goNumber
hi def link goBinNumber       goNumber
hi def link goIdentifierPrime goIdentifier
hi def link goTrait           goType
hi def link goDeriveTrait     goTrait

hi def link goMacroRepeatCount   goMacroRepeatDelimiters
hi def link goMacroRepeatDelimiters   Macro
hi def link goMacroVariable Define
hi def link goSigil         StorageClass
hi def link goEscape        Special
hi def link goEscapeUnicode goEscape
hi def link goEscapeError   Error
hi def link goStringContinuation Special
hi def link goString        String
hi def link goCharacterInvalid Error
hi def link goCharacterInvalidUnicode goCharacterInvalid
hi def link goCharacter     Character
hi def link goNumber        Number
hi def link goBoolean       Boolean
hi def link goEnum          goType
hi def link goEnumVariant   goConstant
hi def link goConstant      Constant
hi def link goSelf          Constant
hi def link goFloat         Float
hi def link goArrowCharacter goOperator
hi def link goOperator      Operator
hi def link goKeyword       Keyword
hi def link goTypedef       Keyword " More precise is Typedef, but it doesn't feel right for Rust
hi def link goStructure     Keyword " More precise is Structure
hi def link goUnion         goStructure
hi def link goPubScopeDelim Delimiter
hi def link goPubScopeCrate clangKeyword
hi def link goSuper         clangKeyword
hi def link goReservedKeyword Error
hi def link goRepeat        Conditional
hi def link goConditional   Conditional
hi def link goIdentifier    Identifier
hi def link goCapsIdent     clangIdentifier
hi def link goModPath       Include
hi def link goModPathSep    Delimiter
hi def link goFunction      Function
hi def link goFuncName      Function
hi def link goFuncCall      Function
hi def link goShebang       Comment
hi def link goCommentLine   Comment
hi def link goCommentLineDoc SpecialComment
hi def link goCommentLineDocError Error
hi def link goCommentBlock  clangCommentLine
hi def link goCommentBlockDoc clangCommentLineDoc
hi def link goCommentBlockDocError Error
hi def link goAssert        PreCondit
hi def link goPanic         PreCondit
hi def link goMacro         Macro
hi def link goType          Type
hi def link goTodo          Todo
hi def link goAttribute     PreProc
hi def link goDerive        PreProc
hi def link goDefault       StorageClass
hi def link goStorage       StorageClass
hi def link goObsoleteStorage Error
hi def link goLifetime      Special
hi def link goLabel         Label
hi def link goInvalidBareKeyword Error
hi def link goExternCrate   clangKeyword
hi def link goObsoleteExternMod Error
hi def link goBoxPlacementParens Delimiter
hi def link goQuestionMark  Special

" Other Suggestions:
" hi goAttribute ctermfg=cyan
" hi goDerive ctermfg=cyan
" hi goAssert ctermfg=yellow
" hi goPanic ctermfg=red
" hi goMacro ctermfg=magenta

syn sync minlines=200
syn sync maxlines=500

let b:current_syntax = "go"
