" Vim syntax file
" Language:     Rust
" Maintainer:   Patrick Walton <pcwalton@mozilla.com>
" Maintainer:   Ben Blum <bblum@cs.cmu.edu>
" Maintainer:   Chris Morgan <me@chrismorgan.info>
" Last Change:  Feb 24, 2016
" For bugs, patches and license go to https://github.com/clang-lang/clang.vim

if version < 600
	syntax clear
elseif exists("b:current_syntax")
	finish
endif

" Syntax definitions {{{1
" Basic keywords {{{2
syn keyword   clangConditional match if else switch
syn keyword   clangRepeat for loop while do
syn keyword   clangTypedef type nextgroup=clangIdentifier skipwhite skipempty
syn keyword   clangStructure struct enum nextgroup=clangIdentifier skipwhite skipempty
syn keyword   clangUnion union nextgroup=clangIdentifier skipwhite skipempty contained
syn match clangUnionContextual /\<union\_s\+\%([^[:cntrl:][:space:][:punct:][:digit:]]\|_\)\%([^[:cntrl:][:punct:][:space:]]\|_\)*/ transparent contains=clangUnion
syn keyword   clangOperator    as

syn match     clangAssert      "\<assert\(\w\)*!" contained
syn match     clangPanic       "\<panic\(\w\)*!" contained
syn keyword   clangKeyword     break virtual override template any typename interface where generic early forward class extension  
syn keyword   clangKeyword     box nextgroup=clangBoxPlacement skipwhite skipempty
syn keyword   clangKeyword     continue new property free group module import with
syn keyword   clangKeyword     extern nextgroup=clangExternCrate,clangObsoleteExternMod skipwhite skipempty
syn keyword   clangKeyword     fun nextgroup=clangFuncName skipwhite skipempty
syn keyword   clangKeyword     in Init Defer let var is private protected public include safe 
syn keyword		clangKeyword		 this init auto friend class space namespace group  
syn keyword   clangKeyword     final defer sealed delete extension val
syn keyword   clangKeyword     public  nextgroup=clangPubScope skipwhite skipempty
syn keyword   clangKeyword     return void any using nullptr null can blank  
syn keyword   clangSuper       super
syn keyword   clangKeyword     unsafe where safe own raw give pull mutable inline case switch default
syn keyword   clangKeyword     use nextgroup=clangModPath skipwhite skipempty
" FIXME: Scoped impl's name is also fallen in this category
syn keyword   clangKeyword     mod trait nextgroup=clangIdentifier skipwhite skipempty
syn keyword   clangStorage     move mut ref static const static_cast
syn match clangDefault /\<default\ze\_s\+\(impl\|fn\|type\|const\)\>/

syn keyword   clangInvalidBareKeyword crate

syn keyword clangPubScopeCrate crate contained
syn match clangPubScopeDelim /[()]/ contained
syn match clangPubScope /([^()]*)/ contained contains=clangPubScopeDelim,clangPubScopeCrate,clangSuper,clangModPath,clangModPathSep,clangSelf transparent

syn keyword   clangExternCrate crate contained nextgroup=clangIdentifier,clangExternCrateString skipwhite skipempty
" This is to get the `bar` part of `extern crate "foo" as bar;` highlighting.
syn match   clangExternCrateString /".*"\_s*as/ contained nextgroup=clangIdentifier skipwhite transparent skipempty contains=clangString,clangOperator
syn keyword   clangObsoleteExternMod mod contained nextgroup=clangIdentifier skipwhite skipempty

syn match     clangIdentifier  contains=clangIdentifierPrime "\%([^[:cntrl:][:space:][:punct:][:digit:]]\|_\)\%([^[:cntrl:][:punct:][:space:]]\|_\)*" display contained
syn match     clangFuncName    "\%([^[:cntrl:][:space:][:punct:][:digit:]]\|_\)\%([^[:cntrl:][:punct:][:space:]]\|_\)*" display contained

syn region    clangBoxPlacement matchgroup=clangBoxPlacementParens start="(" end=")" contains=TOP contained
" Ideally we'd have syntax rules set up to match arbitrary expressions. Since
" we don't, we'll just define temporary contained rules to handle balancing
" delimiters.
syn region    clangBoxPlacementBalance start="(" end=")" containedin=clangBoxPlacement transparent
syn region    clangBoxPlacementBalance start="\[" end="\]" containedin=clangBoxPlacement transparent
" {} are handled by clangFoldBraces

syn region clangMacroRepeat matchgroup=clangMacroRepeatDelimiters start="$(" end=")" contains=TOP nextgroup=clangMacroRepeatCount
syn match clangMacroRepeatCount ".\?[*+]" contained
syn match clangMacroVariable "$\w\+"

" Reserved (but not yet used) keywords {{{2
syn keyword   clangReservedKeyword alignof become offsetof priv pure sizeof typeof unsized yield abstract  macro

" Built-in types {{{2
syn keyword   clangType        isize usize char bool u8 u16 u32 u64 u128 f32
syn keyword   clangType        f64 i8 i16 i32 i64 i128 string this int uint8_t uint32_t  unsigned 

" Things from the libstd v1 prelude (src/libstd/prelude/v1.rs) {{{2
" This section is just straight transformation of the contents of the prelude,
" to make it easy to update.

" Reexported core operators {{{3
syn keyword   clangTrait       Copy Send Sized Sync
syn keyword   clangTrait       Drop Fn FnMut FnOnce

" Reexported functions {{{3
" There’s no point in highlighting these; when one writes drop( or drop::< it
" gets the same highlighting anyway, and if someone writes `let drop = …;` we
" don’t really want *that* drop to be highlighted.
"syn keyword clangFunction drop

" Reexported types and traits {{{3
"syn keyword clangTrait Box
"syn keyword clangTrait ToOwned
"syn keyword clangTrait Clone
"syn keyword clangTrait PartialEq PartialOrd Eq Ord
"syn keyword clangTrait AsRef AsMut Into From
"syn keyword clangTrait Default
"syn keyword clangTrait Iterator Extend IntoIterator
"syn keyword clangTrait DoubleEndedIterator ExactSizeIterator
"syn keyword clangEnum Option
"syn keyword clangEnumVariant Some None
"syn keyword clangEnum Result
"syn keyword clangEnumVariant Ok Err
"syn keyword clangTrait SliceConcatExt
"syn keyword clangTrait String ToString
"syn keyword clangTrait Vec

" Other syntax {{{2
syn keyword   clangSelf        self
syn keyword   clangBoolean     true false

" If foo::bar changes to foo.bar, change this ("::" to "\.").
" If foo::bar changes to Foo::bar, change this (first "\w" to "\u").
syn match     clangModPath     "\w\(\w\)*::[^<]"he=e-3,me=e-3
syn match     clangModPathSep  "::"

syn match     clangFuncCall    "\w\(\w\)*("he=e-1,me=e-1
syn match     clangFuncCall    "\w\(\w\)*::<"he=e-3,me=e-3 " foo::<T>();

" This is merely a convention; note also the use of [A-Z], restricting it to
" latin identifiers rather than the full Unicode uppercase. I have not used
" [:upper:] as it depends upon 'noignorecase'
"syn match     clangCapsIdent    display "[A-Z]\w\(\w\)*"

syn match     clangOperator     display "\%(+\|-\|/\|*\|=\|\^\|&\||\|!\|>\|<\|%\)=\?"
" This one isn't *quite* right, as we could have binary-& with a reference
syn match     clangSigil        display /&\s\+[&~@*][^)= \t\r\n]/he=e-1,me=e-1
syn match     clangSigil        display /[&~@*][^)= \t\r\n]/he=e-1,me=e-1
" This isn't actually correct; a closure with no arguments can be `|| { }`.
" Last, because the & in && isn't a sigil
syn match     clangOperator     display "&&\|||"
" This is clangArrowCharacter rather than clangArrow for the sake of matchparen,
" so it skips the ->; see http://stackoverflow.com/a/30309949 for details.
syn match     clangArrowCharacter display "->"
syn match     clangQuestionMark display "?\([a-zA-Z]\+\)\@!"

syn match     clangMacro       '\w\(\w\)*!' contains=clangAssert,clangPanic
syn match     clangMacro       '#\w\(\w\)*' contains=clangAssert,clangPanic

syn match     clangEscapeError   display contained /\\./
syn match     clangEscape        display contained /\\\([nrt0\\'"]\|x\x\{2}\)/
syn match     clangEscapeUnicode display contained /\\u{\x\{1,6}}/
syn match     clangStringContinuation display contained /\\\n\s*/
syn region    clangString      start=+b"+ skip=+\\\\\|\\"+ end=+"+ contains=clangEscape,clangEscapeError,clangStringContinuation
syn region    clangString      start=+"+ skip=+\\\\\|\\"+ end=+"+ contains=clangEscape,clangEscapeUnicode,clangEscapeError,clangStringContinuation,@Spell
syn region    clangString      start='b\?r\z(#*\)"' end='"\z1' contains=@Spell

syn region    clangAttribute   start="#!\?\[" end="\]" contains=clangString,clangDerive,clangCommentLine,clangCommentBlock,clangCommentLineDocError,clangCommentBlockDocError
syn region    clangDerive      start="derive(" end=")" contained contains=clangDeriveTrait
" This list comes from src/libsyntax/ext/deriving/mod.rs
" Some are deprecated (Encodable, Decodable) or to be removed after a new snapshot (Show).
syn keyword   clangDeriveTrait contained Clone Hash RustcEncodable RustcDecodable Encodable Decodable PartialEq Eq PartialOrd Ord Rand Show Debug Default FromPrimitive Send Sync Copy

" Number literals
syn match     clangDecNumber   display "\<[0-9][0-9_]*\%([iu]\%(size\|8\|16\|32\|64\|128\)\)\="
syn match     clangHexNumber   display "\<0x[a-fA-F0-9_]\+\%([iu]\%(size\|8\|16\|32\|64\|128\)\)\="
syn match     clangOctNumber   display "\<0o[0-7_]\+\%([iu]\%(size\|8\|16\|32\|64\|128\)\)\="
syn match     clangBinNumber   display "\<0b[01_]\+\%([iu]\%(size\|8\|16\|32\|64\|128\)\)\="

" Special case for numbers of the form "1." which are float literals, unless followed by
" an identifier, which makes them integer literals with a method call or field access,
" or by another ".", which makes them integer literals followed by the ".." token.
" (This must go first so the others take precedence.)
syn match     clangFloat       display "\<[0-9][0-9_]*\.\%([^[:cntrl:][:space:][:punct:][:digit:]]\|_\|\.\)\@!"
" To mark a number as a normal float, it must have at least one of the three things integral values don't have:
" a decimal point and more numbers; an exponent; and a type suffix.
syn match     clangFloat       display "\<[0-9][0-9_]*\%(\.[0-9][0-9_]*\)\%([eE][+-]\=[0-9_]\+\)\=\(f32\|f64\)\="
syn match     clangFloat       display "\<[0-9][0-9_]*\%(\.[0-9][0-9_]*\)\=\%([eE][+-]\=[0-9_]\+\)\(f32\|f64\)\="
syn match     clangFloat       display "\<[0-9][0-9_]*\%(\.[0-9][0-9_]*\)\=\%([eE][+-]\=[0-9_]\+\)\=\(f32\|f64\)"

" For the benefit of delimitMate
syn region clangLifetimeCandidate display start=/&'\%(\([^'\\]\|\\\(['nrt0\\\"]\|x\x\{2}\|u{\x\{1,6}}\)\)'\)\@!/ end=/[[:cntrl:][:space:][:punct:]]\@=\|$/ contains=clangSigil,clangLifetime
syn region clangGenericRegion display start=/<\%('\|[^[cntrl:][:space:][:punct:]]\)\@=')\S\@=/ end=/>/ contains=clangGenericLifetimeCandidate
syn region clangGenericLifetimeCandidate display start=/\%(<\|,\s*\)\@<='/ end=/[[:cntrl:][:space:][:punct:]]\@=\|$/ contains=clangSigil,clangLifetime

"clangLifetime must appear before clangCharacter, or chars will get the lifetime highlighting
syn match     clangLifetime    display "\'\%([^[:cntrl:][:space:][:punct:][:digit:]]\|_\)\%([^[:cntrl:][:punct:][:space:]]\|_\)*"
syn match     clangLabel       display "\'\%([^[:cntrl:][:space:][:punct:][:digit:]]\|_\)\%([^[:cntrl:][:punct:][:space:]]\|_\)*:"
syn match   clangCharacterInvalid   display contained /b\?'\zs[\n\r\t']\ze'/
" The groups negated here add up to 0-255 but nothing else (they do not seem to go beyond ASCII).
syn match   clangCharacterInvalidUnicode   display contained /b'\zs[^[:cntrl:][:graph:][:alnum:][:space:]]\ze'/
syn match   clangCharacter   /b'\([^\\]\|\\\(.\|x\x\{2}\)\)'/ contains=clangEscape,clangEscapeError,clangCharacterInvalid,clangCharacterInvalidUnicode
syn match   clangCharacter   /'\([^\\]\|\\\(.\|x\x\{2}\|u{\x\{1,6}}\)\)'/ contains=clangEscape,clangEscapeUnicode,clangEscapeError,clangCharacterInvalid

syn match clangShebang /\%^#![^[].*/
syn region clangCommentLine                                                  start="//"                      end="$"   contains=clangTodo,@Spell
syn region clangCommentLineDoc                                               start="//\%(//\@!\|!\)"         end="$"   contains=clangTodo,@Spell
syn region clangCommentLineDocError                                          start="//\%(//\@!\|!\)"         end="$"   contains=clangTodo,@Spell contained
syn region clangCommentBlock             matchgroup=clangCommentBlock         start="/\*\%(!\|\*[*/]\@!\)\@!" end="\*/" contains=clangTodo,clangCommentBlockNest,@Spell
syn region clangCommentBlockDoc          matchgroup=clangCommentBlockDoc      start="/\*\%(!\|\*[*/]\@!\)"    end="\*/" contains=clangTodo,clangCommentBlockDocNest,@Spell
syn region clangCommentBlockDocError     matchgroup=clangCommentBlockDocError start="/\*\%(!\|\*[*/]\@!\)"    end="\*/" contains=clangTodo,clangCommentBlockDocNestError,@Spell contained
syn region clangCommentBlockNest         matchgroup=clangCommentBlock         start="/\*"                     end="\*/" contains=clangTodo,clangCommentBlockNest,@Spell contained transparent
syn region clangCommentBlockDocNest      matchgroup=clangCommentBlockDoc      start="/\*"                     end="\*/" contains=clangTodo,clangCommentBlockDocNest,@Spell contained transparent
syn region clangCommentBlockDocNestError matchgroup=clangCommentBlockDocError start="/\*"                     end="\*/" contains=clangTodo,clangCommentBlockDocNestError,@Spell contained transparent
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

syn keyword clangTodo contained TODO FIXME XXX NB NOTE

" Folding rules {{{2
" Trivial folding rules to begin with.
" FIXME: use the AST to make really good folding
syn region clangFoldBraces start="{" end="}" transparent fold

" Default highlighting {{{1
hi def link clangDecNumber       clangNumber
hi def link clangHexNumber       clangNumber
hi def link clangOctNumber       clangNumber
hi def link clangBinNumber       clangNumber
hi def link clangIdentifierPrime clangIdentifier
hi def link clangTrait           clangType
hi def link clangDeriveTrait     clangTrait

hi def link clangMacroRepeatCount   clangMacroRepeatDelimiters
hi def link clangMacroRepeatDelimiters   Macro
hi def link clangMacroVariable Define
hi def link clangSigil         StorageClass
hi def link clangEscape        Special
hi def link clangEscapeUnicode clangEscape
hi def link clangEscapeError   Error
hi def link clangStringContinuation Special
hi def link clangString        String
hi def link clangCharacterInvalid Error
hi def link clangCharacterInvalidUnicode clangCharacterInvalid
hi def link clangCharacter     Character
hi def link clangNumber        Number
hi def link clangBoolean       Boolean
hi def link clangEnum          clangType
hi def link clangEnumVariant   clangConstant
hi def link clangConstant      Constant
hi def link clangSelf          Constant
hi def link clangFloat         Float
hi def link clangArrowCharacter clangOperator
hi def link clangOperator      Operator
hi def link clangKeyword       Keyword
hi def link clangTypedef       Keyword " More precise is Typedef, but it doesn't feel right for Rust
hi def link clangStructure     Keyword " More precise is Structure
hi def link clangUnion         clangStructure
hi def link clangPubScopeDelim Delimiter
hi def link clangPubScopeCrate clangKeyword
hi def link clangSuper         clangKeyword
hi def link clangReservedKeyword Error
hi def link clangRepeat        Conditional
hi def link clangConditional   Conditional
hi def link clangIdentifier    Identifier
hi def link clangCapsIdent     clangIdentifier
hi def link clangModPath       Include
hi def link clangModPathSep    Delimiter
hi def link clangFunction      Function
hi def link clangFuncName      Function
hi def link clangFuncCall      Function
hi def link clangShebang       Comment
hi def link clangCommentLine   Comment
hi def link clangCommentLineDoc SpecialComment
hi def link clangCommentLineDocError Error
hi def link clangCommentBlock  clangCommentLine
hi def link clangCommentBlockDoc clangCommentLineDoc
hi def link clangCommentBlockDocError Error
hi def link clangAssert        PreCondit
hi def link clangPanic         PreCondit
hi def link clangMacro         Macro
hi def link clangType          Type
hi def link clangTodo          Todo
hi def link clangAttribute     PreProc
hi def link clangDerive        PreProc
hi def link clangDefault       StorageClass
hi def link clangStorage       StorageClass
hi def link clangObsoleteStorage Error
hi def link clangLifetime      Special
hi def link clangLabel         Label
hi def link clangInvalidBareKeyword Error
hi def link clangExternCrate   clangKeyword
hi def link clangObsoleteExternMod Error
hi def link clangBoxPlacementParens Delimiter
hi def link clangQuestionMark  Special

" Other Suggestions:
" hi clangAttribute ctermfg=cyan
" hi clangDerive ctermfg=cyan
" hi clangAssert ctermfg=yellow
" hi clangPanic ctermfg=red
" hi clangMacro ctermfg=magenta

syn sync minlines=200
syn sync maxlines=500

let b:current_syntax = "clang"
