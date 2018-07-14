import com.concur.*

pipeline = new Commands()

public cMake(Map yml, Map args) {
  String cMakeListsLocation = args?.cMakeListsLocation  ?: '..'
  String cMakeBuildDir      = args?.cMakeBuildDir ?: yml?.tools?.cPlusPlus?.cMakeBuildDir
  String container          = args?.container     ?: yml?.tools?.cPlusPlus?.container

  assert cMakeBuildDir  : "Workfkows :: cPlusPlus :: cMake :: [cMakeDir] needs to be defined."
  assert container : "Workfkows :: cMake :: ccPlusPlusMake :: [container] needs to be defined."
  
  String runString = "cd ${cMakeBuildDir} && cmake ${cMakeListsLocation}"
  pipeline.debugPrint(runString)

  docker.image(container).inside("-u 0:0 --entrypoint=''") {
    // sh runString
    sh "ls"
  }
}

public make(Map yml, Map args) {
  String makeDir   = args?.makeDir   ?: yml?.tools?.cMake?.cMakeDir
  String container = args?.container ?: yml?.tools?.container

  assert makeDir   : "Workfkows :: cPlusPlus :: make :: [makeDir] needs to be defined."
  assert container : "Workfkows :: cPlusPlus :: make :: [container] needs to be defined."

  docker.image(container).inside("-u 0:0 --entrypoint=''") {
    sh "cd ${makeDir} && make"
  }
}

return this;