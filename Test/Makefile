.PHONY: default
default: generated_sources

.PHONY: vsetup
vsetup:
	test -d venv || virtualenv venv

.PHONY: check_tools
check_tools:
ifndef DONT_UPDATE_CASTLETOOLS
	castletools-update
endif
	components install

RIVIERA_LPMSERVICE_DIR = $(shell components get RivieraLpmService-x86_64 installed_location)

.PHONY: generated_sources
generated_sources: check_tools
	rm -rf builds
	mkdir -p builds
	ln -nsf $(RIVIERA_LPMSERVICE_DIR) RivieraLpmService

.PHONY: comptest
comptest:
	pip install -qr requirements.txt
	pytest -sv --junitxml=/tmp/output.xml

.PHONY:
covgen:
	gcovr -r ../ -e builds -e ../AVS/test --branches --xml -o /tmp/coverage.xml

.PHONY: clean
clean: clean-build clean-pyc clean-test
	rm -rvf ./*.tgz ./venv ./protobufs ./*.log ./*.xml builds/

## Remove Python build artifacts
.PHONY: clean-build
clean-build:
	rm -vrf ./build/
	rm -vrf ./dist/
	rm -vrf ./.eggs/
	rm -vrf ./*.egg-info

## Remove Python file artifacts and binaries
.PHONY: clean-pyc
clean-pyc:
	find . -name '*.pyc' -delete -print
	find . -name '*.pyo' -delete -print
	find . -name '*~' -delete -print
	find . -name '__pycache__' -delete -print

## Remove Python test and Code coverage artifacts
.PHONY: clean-test
clean-test:
	rm -vrf .tox/
	rm -f .coverage
	rm -vrf htmlcov/
	rm -vrf ./tests/.cache
	rm -vrf .pytest_cache

.PHONY: distclean
distclean:
	git clean -fdX

