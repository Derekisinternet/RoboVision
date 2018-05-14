// node {
//     checkout scm
//     stage('Make'){
//         docker.image().inside {
//             sh 'make'
//         }
//     }
//     stage('Test') {
//         docker.image.inside {
//             sh './roboVision -t ../training_data'
//         }
//     }
// }

library "workflowLibs@v0.6.0"

plWorkflow {
  notify = false
}

