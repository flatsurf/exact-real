#!/bin/bash
set -e

! grep "TO""DO" `git ls-files | grep -v external | grep -v azure-pipelines`
