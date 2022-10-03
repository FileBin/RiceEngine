# Scene

## 1. Structure

  ```mermaid
  classDiagram
  class SceneBase {
    -uint64 uuid_counter
    -umap~uint64, ptrSceneObjectBase~ all_objects
    -umap~ uint64, uptrComponentDeserializer~ component_deserializers
    -ptr~Object~ root
    -ptr~EngineBase~ engine
    -bool loaded
  }
  ```

  > Component contains 