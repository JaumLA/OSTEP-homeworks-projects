#!/bin/bash

n=1
while [ $n -le 262144 ] 
do
  echo "With $n pages"
  ./tlb n 5
  let n=n*2
done
let n=1