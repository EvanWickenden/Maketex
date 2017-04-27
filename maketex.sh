#!/bin/sh

die=0


pdflatex $1.tex || die=1

echo $'\n\n\n\n\n\n _____ pdflatex output completed _____ \n\n\n\n\n\n'

rm -f $1.{log,aux,dvi} 

if [ $die -eq 1 ] ;
then 
    exit 1
fi

open $1.pdf

