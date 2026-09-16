savedcmd_kmetrics.mod := printf '%s\n'   kmetrics.o | awk '!x[$$0]++ { print("./"$$0) }' > kmetrics.mod
