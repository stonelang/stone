"execute pathogen#infect()
filetype plugin indent on
syn on se title
"autocmd vimenter * NERDTree


" set UTF-8 encoding
set enc=utf-8
set fenc=utf-8
set termencoding=utf-8

" disable vi compatibility (emulation of old bugs)
set nocompatible


set tabstop=2
set softtabstop=2
set shiftwidth=2
set noexpandtab
set timeoutlen=1000
set ttimeoutlen=0
set t_Co=256
set t_ut=
set background=dark
colorscheme lucius


" intelligent comments
set comments=sl:/*,mb:\ *,elx:\ */

" Enhanced keyboard mappings
"
" in normal mode F2 will save the file
nmap <F2> :w<CR>
" in insert mode F2 will exit insert, save, enters insert again
imap <F2> <ESC>:w<CR>i
" switch between header/source with F4
map <F4> :e %:p:s,.h$,.X123X,:s,.cpp$,.h,:s,.X123X$,.cpp,<CR>
" recreate tags file with F5
map <F5> :!ctags -R –c++-kinds=+p –fields=+iaS –extra=+q .<CR>
" create doxygen comment
map <F6> :Dox<CR>
" build using makeprg with <F7>
map <F7> :make<CR>
" build using makeprg with <S-F7>
map <S-F7> :make clean all<CR>
" goto definition with F12
map <F12> <C-]>



set clipboard=unnamedplus
"colorscheme visualstudio
set wildmenu
set mouse=a
set showmatch
set number
set autoindent 
set smarttab
set foldmethod=manual
set cpo-=<
set wcm=<c-Z>
set backspace=2
set smartindent
set backupdir=~/.vimtemp
set dir=~/.vimtemp


set tags=tags
