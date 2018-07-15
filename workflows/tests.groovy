import com.concur.*

public test(Map yml, Map args) {
  String exe         = args?.executable  ?: yml.tools?.tests?.executable
  String positiveSet = args?.positveSet  ?: yml.tools?.tests?.positiveSet
  String negativeSet = args?.negativeSet ?: yml.tools?.tests?.negativeSet
  String dockerImage = args?.dockerImage ?: yml.tools?.tests?.dockerImage
  String cMakeListsLocation = args?.cMakeListsLocation  ?: '..'
  String imageSetDir = args?.imageSetDir ?: yml.tools?.tests?.imageSetDir

  assert exe : "workflows :: tests :: test :: [executable] required as a parameter"
  assert dockerImage : "workflows :: tests :: test :: [dockerImage] required as a parameter"
  assert positiveSet : "workflows :: tests :: test :: [positiveSet] required as a parameter"
  assert negativeSet : "workflows :: tests :: test :: [negativeSet] required as a parameter"

  String runString = "./makeCMakeLists.sh -r && mkdir build && cd build"
  runString += " && echo 'about to run cmake'"
  runString += " && cmake $cMakeListsLocation"
  runString += " && echo 'about to run make'"
  runString += " && make ."
  runString += " && echo 'about to train a model'"
  runString += " && ./$exe -t $imageSetDir/$positiveSet -n $imageSetDir/$negativeSet"
  docker.image(dockerImage).inside("-u 0:0 --entrypoint=''"){
    sh runString
  }
  
}

return this;
