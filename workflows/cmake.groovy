import com.concur.*

public cMake(Map, yml, Map args) {
  String cMakeLists = args?.cMakeLists ?: yml?.tools?.cMake?.cMakeLists
  String cMakeDir   = args?.cMakeDir   ?: yml?.tools?.cMake?.cMakeDir
  String container  = args?.container  ?: yml?.tools?.cMake

  assert cMakeDir : "Workfkows :: cMake :: cMake :: [cMakeDir] needs to be defined."
  sh "cd ${cMakeDir} && cmake ."
}

public make(Map yml, Map args) {
  String makeDir   = args?.makeDir   ?: yml?.tools?.cMake?.cMakeDir
  String container = args?.container ?: yml?.tools?.container

  assert makeDir : "Workfkows :: cMake :: cMake :: [makeDir] needs to be defined."
  sh "cd ${makeDir} && make"
}

return this;