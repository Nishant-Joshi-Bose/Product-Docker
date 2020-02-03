.PHONY: default
default: generated_sources

.PHONY: vsetup
vsetup:
	test -d venv || virtualenv venv

RIVIERA_LPMSERVICE_DIR = $(shell components get RivieraLpmService-x86_64 installed_location)
CLI_SERVICE_DIR = $(shell components get Cli-x86_64 installed_location)
AUDIOSOURCE_DIR = $(shell components get AudioSource-x86_64 installed_location)
BOSELIBS_DIR = $(shell components get BoseLibs-x86_64 installed_location)
SOUNDTOUCHSDK_DIR = $(shell components get CAPSService-x86_64 installed_location)
AUDIOPATHCLIENT_DIR = $(shell components get AudioPathClient-x86_64 installed_location)
#AUDIOPATHSERVICE_DIR = $(shell components get AudioPathService-x86_64 installed_location) 
AUDIOPATHPRODUCTS_DIR = $(shell components get AudioPathProducts-x86_64 installed_location)

.PHONY: generated_sources
generated_sources:
	rm -rf /scratch/builds
	rm -rf builds/
	mkdir -p /scratch/builds
	mkdir -p builds/
	cp -r $(RIVIERA_LPMSERVICE_DIR) /scratch/builds
	cp -r $(CLI_SERVICE_DIR) /scratch/builds
	cp -r $(SOUNDTOUCHSDK_DIR) /scratch/builds
	cp -r $(BOSELIBS_DIR) /scratch/builds
	cp -r $(AUDIOSOURCE_DIR) /scratch/builds
	cp -r $(AUDIOPATHCLIENT_DIR) /scratch/builds/
	#cp -r $(AUDIOPATHSERVICE_DIR) /scratch/builds/
	cp -r $(AUDIOPATHPRODUCTS_DIR) /scratch/builds/
	ln -nsf /scratch/builds/Cli-x86_64 ./builds/CliService

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

