#!/bin/bash
test -n "$(./services/eval/evalclient evalservice | grep response)"
