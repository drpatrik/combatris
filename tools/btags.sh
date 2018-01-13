#!/bin/bash
ctags -e -R --fields="+afikKlmnsSzt" --extra=+fq --exclude=db --exclude=.git --exclude=public -f TAGS
