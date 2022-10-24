# Graphics Manager

## 1. About

Grapics Manager is class that manages all graphics objects in the Engine. It allocates them in defined order and destroys them in backwards order.

## 2. Graphics Manager Init (Vulkan toolchain)

```mermaid
flowchart TB
    1(Start) --> 2
    subgraph Init
        2[Init Vulkan instance] --> 3
        3[Init Vulkan Debug Messenger] --> 4
        4[Find suitable physical device] --> 5
        5{Device is found?} --->|Yes| 6
        6[Create Logical Device] --> 7
        7[Create Graphics Queue] --> 8
        8[Init Swapchain] --> 9
        9[Init Commands] --> 10
        10[Init Descriptor Pool] --> 11
        11[Init Depth Image] --> 12
        12[Init Default renderpass] --> 13
        13[Init Framebuffers] --> 14
    end
    14[Init Sync Structures] --> 15(End)
    5 -->|No| Err1[Throw Vulkan Exception]

```

## 3. Graphics Manager Cleanup (Vulkan toolchain)

```mermaid
flowchart TB
    Start(Start) --> 2
    subgraph Cleanup
        2[Wait for Rendered Frames] --> 3
        3[Destroy command buffers] --> 4
        4[Destroy other Graphics Components] --> 5
        5[Create Logical Device] --> 6
        6[Destroy Descriptor Pool] --> 7
        7[Destroy Swapchain] --> 8
        8[Destroy Default Render Pass] --> 9
        9[Destroy sync structures] --> 10
        10[Destroy Device]
    end
    10 --> End(End)

```
