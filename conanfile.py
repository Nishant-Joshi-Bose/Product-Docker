from conans import ConanFile, python_requires, tools
import os

common = python_requires('CastleConanRecipes/[>=0.0.22-0, include_prerelease=True]@BoseCorp/master')

class ProfessorDocker(common.MakefilePackage):
    name = "Product-Docker"
    version = common.get_version_git_tag(prefix="")
    url = "https://github.com/BoseCorp/%s" % name
    scm = {
        "type": "git",
        "url": "git@github.com:BoseCorp/%s.git" % name,
        "revision": "auto"
    }

    x86_64_excludes = ["opensource-OpenAvnuApple", "RivieraSwUpRecovery"]

    def build(self):
        self.run('make cfg=%s sdk=%s jobs=%d DONT_UPDATE_CASTLETOOLS=1' % (self.settings.build_type, self.settings.arch, tools.cpu_count() ) )

    def requirements(self):
        for package in common.parse_requires( os.path.join(os.path.dirname(__file__),"depends.json")):
            assert package[0]
            packageName = package[0].split('/')[0]
            assert packageName

            if self.settings.arch == "x86_64" and packageName in self.x86_64_excludes:
                print("Skipping %s - doesn't support %s" % (package[0],self.settings.arch))
                continue
            else:
                self.requires(*package)

    def package(self):
        buildDir = "builds/Product-Docker-%s" % self.settings.arch
        self.copy("*", dst="", src="%s" % (buildDir), keep_path=True)
