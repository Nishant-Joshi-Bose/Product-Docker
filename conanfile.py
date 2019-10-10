from conans import ConanFile, python_requires
import os

common = python_requires('CastleConanRecipes/[>=0.0.22-0, include_prerelease=True]@BoseCorp/master')


class Professor(common.MakefilePackage):
    name = "Professor"
    version = common.get_version_git_tag(prefix="professor/")
    generators = "json"
    url = "https://github.com/BoseCorp/%s" % name
    scm = {
        "type": "git",
        "url": "git@github.com:BoseCorp/%s.git" % name,
        "revision": "auto"
    }

    options = {"product_package":[True, False]}
    default_options = {"product_package": False}


    # for x86_64 we only need opensource-opkg
    # for qc64 we need GVA, CastleSASS, and CastleAudioPathClient
    # for qc32 we need to EXCLUDE GVA
    x86_64_depends = ["opensource-opkg"]
    qc8017_64_depends = ["GVA", "CastleSASS", "CastleAudioPathClient"]
    qc8017_32_excludes = ["GVA"]

    def build(self):
        pass

    def requirements(self):
        for package in common.parse_requires(os.path.join(os.path.dirname(__file__),"depends.json")):
            assert package[0]
            packageName = package[0].split('/')[0]
            assert packageName


            # only include the x86_64 depends
            if self.settings.arch == "x86_64":
               if any(inclusion == packageName for inclusion in self.x86_64_depends):
                  print("Adding %s - for x86_64 builds" % package[0])
                  self.requires(*package)

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


    def package_id(self):
        self.info.header_only()
