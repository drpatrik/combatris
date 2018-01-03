#!/bin/bash
ctags -e -R --extra=+fq --exclude=db --exclude=.git --exclude=public -f TAGS
