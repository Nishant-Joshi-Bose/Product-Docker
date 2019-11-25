from conans import python_requires
import os

common = python_requires('CastleConanRecipes/[>=0.0.22-0, include_prerelease=True]@BoseCorp/master')


class Professor(common.MakefilePackage):
    name = "Professor"
    version = common.get_version_git_tag()
    generators = "json"
    url = "https://github.com/BoseCorp/Product-%s" % name
    scm = {
        "type": "git",
        "url": "git@github.com:BoseCorp/Product-%s.git" % name,
        "revision": "auto"
    }

    options = {"product_package":[True, False], "ipks_list":"ANY"}
    default_options = {"product_package": False, "ipks_list":None}

    # for qc64 we need GVA, CastleSASS, and CastleAudioPathClient
    # for qc32 we need to EXCLUDE GVA
    qc8017_64_depends = ["GVA", "CastleSASS", "CastleAudioPathClient", "DinghyWebBrowser"]
    qc8017_32_excludes = ["GVA", "DinghyWebBrowser"]


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
