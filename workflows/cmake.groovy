import com.concur.*

public cMake(Map, yml, Map args) {
  def cMakeLists = args?.cMakeLists ?: yml?.tools?.cMake?.cMakeLists
  def cmakeDir   = args?.cMakeDir   ?: yml?.tools?.cMake?.cMakeDir

  assert cMakeDir : "Workfkows :: cMake :: cMake :: [cMakeDir] needs to be defined."
  sh "cmake ${cMakeDir}"
}

public make(Map yml, Map args) {
  def makeDir = args?.makeDir ?: yml?.tools?.cMake?.cMakeDir

  assert makeDir : "Workfkows :: cMake :: cMake :: [makeDir] needs to be defined."
  sh "make"
}