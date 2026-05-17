// Stack-discipline torture test. Three levels of '{' nesting, each closed by
// '}'. Each '{' pushes onto Stack, each '}' must pop and match. A trailing
// statement after the close verifies Root is properly restored.
//   A{                  Root Mnemo, MnemoAnalysis {
//     B{                Root Mnemo, MnemoAnalysis {
//       C{              Root Mnemo, MnemoAnalysis {
//         X=1 Y=2       two var-inits
//       }               Root }
//       Z=3
//     }                 Root }
//   }                   Root }
//   Done=42             new statement to verify clean Root state
A{B{C{X=1 Y=2} Z=3}}
Done=42
