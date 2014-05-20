#ifndef _DURIN_H
#define _DURIN_H

/* library includes */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

/* global macros */
#define new(type, name)						\
  type name = malloc(sizeof(struct type));			\
  memset(name, 0, sizeof(struct type))

/* durin includes */
#include "repl.h"
#include "script.h"

#endif
