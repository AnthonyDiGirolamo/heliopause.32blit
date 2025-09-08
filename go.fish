#!/usr/bin/env fish
cd $(dirname $(readlink -f $(status filename)))

if not test -d venv
    python -m venv venv
end

. ./venv/bin/activate.fish

if not test -f venv/bin/32blit
    pip install 32blit
end

make build-host build-stm32 build-pico
