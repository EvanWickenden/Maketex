#!/bin/sh

if [ $# -ne 1 ] ; then
  exit 1
fi

DIE=0

pdflatex $1.tex || DIE=1

echo $'\n\n\n\n\n\n _____ pdflatex output completed _____ \n\n\n\n\n\n'

rm -f $1.{log,aux,dvi} 

if [ $DIE -eq 1 ] ; then
  exit 1
fi

open $1.pdf

