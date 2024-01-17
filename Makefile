VERSION := 0.1.0
BUILDER_TAG := label_tool_builder:$(VERSION)
DOCKER ?= docker
BUILDX ?=
BUILD_OPTS ?=
RUN_OPTS ?=

.PHONY: all
all:
	@echo "Targets:"
	@echo "  builder : Create the Docker container for building the project."
	@echo "  build   : Build the project from a Docker container."

.PHONY: build
build:
	$(DOCKER) run $(RUN_OPTS) --mount type=bind,source=$(PWD),destination=/home/builder/project,readonly=true -p 9150:9150 -it $(BUILDER_TAG)

.PHONY: builder
builder:
	$(DOCKER) $(BUILDX) build $(BUILD_OPTS) . --tag $(BUILDER_TAG)
