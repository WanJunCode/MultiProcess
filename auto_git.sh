#!/bin/bash
echo auto git 一个便利的git提交脚本
git add .
git commit -m $1
git push origin master
