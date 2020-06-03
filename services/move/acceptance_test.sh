#!/bin/bash
test -n "$(./services/move/moveclient moveservice | grep response)"