MELLOW ?= mellow
PROFILE ?= dev

.PHONY: build

build:
	$(MELLOW) build --profile $(PROFILE)
