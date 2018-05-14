import com.concur.*

public cMake(Map, yml, Map args) {
  def cMakeLists = args?.cMakeLists ?: yml?.tools?.cMake?.cMakeLists
  def cMakeDir   = args?.cMakeDir   ?: yml?.tools?.cMake?.cMakeDir

  assert cMakeDir : "Workfkows :: cMake :: cMake :: [cMakeDir] needs to be defined."
  sh "cd ${cMakeDir} && cmake ."
}

public make(Map yml, Map args) {
  def makeDir = args?.makeDir ?: yml?.tools?.cMake?.cMakeDir

  assert makeDir : "Workfkows :: cMake :: cMake :: [makeDir] needs to be defined."
  sh "cd ${makeDir} && make"
}