import com.concur.*

public test(Map yml, Map args) {
  String exe         = args?.executable  ?: yml?.tools?.tests?.executable
  String positiveSet = args?.positveSet  ?: yml?.tools?.tests?.positiveSet
  String negativeSet = args?.negativeSet ?: yml?.tools?.tests?.negativeSet
  String container

  assert exe : "workflows :: tests :: test :: [executable] required as a parameter"
  assert positiveSet : "workflows :: tests :: test :: [positiveSet] required as a parameter"
  assert negativeSet : "workflows :: tests :: test :: [negativeSet] required as a parameter"

  sh "${exe} -t ${positiveSet} -n ${negativeSet}"
}

return this;
