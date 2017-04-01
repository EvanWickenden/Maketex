#!/bin/sh


DIE=0

pdflatex -halt-on-error $1.tex || DIE=1

echo $'\n\n\n\n\n\n _____ pdflatex output completed _____ \n\n\n\n\n\n'

rm -f $1.{log,aux,dvi} 

if [ $DIE -eq 1 ] ; then
  exit 1
fi

open $1.pdf

