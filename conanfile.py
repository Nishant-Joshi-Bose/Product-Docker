from conans import python_requires
import os, time, calendar
from semver import SemVer

common = python_requires('CastleConanRecipes/[>=0.0.22-0, include_prerelease=True]@BoseCorp/master')


def get_version(git_tag):
    version_file = "version.txt"
    if not os.path.exists(version_file):
        print("Regenerating version.txt file")
        if(git_tag is None):
            # Sometimes this can happen when exporting sources, we already exported the version so we dont need to worry
            return None
        semver = SemVer(git_tag, loose=True)
        build_info_delim = '.'

        # formats the version to be something like 7.0.0.<timestamp>+<buildinfo> where build info will be something like
        # gasdd.1234
        current_time = calendar.timegm(time.gmtime())
        version = "%s.%s.%s-%s+%s" % (semver.major, semver.minor, semver.patch, current_time, build_info_delim.join(semver.build))

        # write the version to a file
        out_file = open(version_file, "w")
        out_file.write(version)
        out_file.close()

        return version
    else:
        # read from the file if we arent trying to regenerate the version
        file = open(version_file, "r")
        version = file.read()
        file.close()
        return version


class Professor(common.MakefilePackage):
    name = "Professor"
    version = None
    generators = "json"
    url = "https://github.com/BoseCorp/Product-%s" % name
    scm = {
        "type": "git",
        "url": "git@github.com:BoseCorp/Product-%s.git" % name,
        "revision": "auto"
    }

    exports = "depends.json", "dev_depends.json", "build_requires.json", "version.txt"

    options = {"product_package":[True, False], "ipks_list":"ANY"}
    default_options = {"product_package": False, "ipks_list":None}

    # for qc64 we need GVA, CastleSASS, and CastleAudioPathClient
    # for qc32 we need to EXCLUDE GVA
    qc8017_64_depends = ["GVA", "CastleSASS", "CastleAudioPathClient", "DinghyWebBrowser"]
    qc8017_32_excludes = ["GVA", "DinghyWebBrowser"]


    def set_version(self):
        self.version = get_version(common.get_version_git_tag())


    def build_requirements(self):
        if self.settings.arch == "x86_64":
            self.build_requires("opensource-opkg/[>=0.0.1-0, include_prerelease=True]@BoseCorp/conan_stable")


    def build(self):
        if self.options.product_package and self.options.ipks_list is not None:
            print("Running conan build steps for product.tar")
            # ipks_list is expected to be a comma separated list, we need to reformat it to a string
            ipks = ''
            for i in self.options.ipks_list.value.split(','):
                ipks += (i + " ")
            self.run("scripts/create-product-tar -i %s" % ipks)


    def requirements(self):
        for package in common.parse_requires(os.path.join(os.path.dirname(__file__),"depends.json")):
            assert package[0]
            packageName = package[0].split('/')[0]
            assert packageName

            # For qc64, only include the ones in the array above
            if self.settings.arch == "qc8017_64":
                if any(inclusion == packageName for inclusion in self.qc8017_64_depends):
                    print("Adding %s - for qc8017_64 builds" % package[0])
                    self.requires(*package)


            # For qc 32, we want to include everything except the above array excludes
            if self.settings.arch == "qc8017_32":
                if any(exception == packageName for exception in self.qc8017_32_excludes):
                    print("Skipping %s - doesn't support %s" % (package[0],self.settings.arch))
                    continue
                self.requires(*package)


    def package(self):
        self.copy("*", dst="", src="builds/deploy", keep_path=True)
        self.copy("*", dst="", src="builds/package_version", keep_path=True)


    def package_id(self):
        self.info.header_only()
