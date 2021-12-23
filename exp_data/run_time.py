import re
from datetime import datetime

start_log = """
     1	21/12/23 01:55:15 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
     2	21/12/23 01:56:19 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
     3	21/12/23 01:57:25 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
     4	21/12/23 01:58:44 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
     5	21/12/23 02:00:23 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
     6	21/12/23 02:02:17 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
     7	21/12/23 02:03:48 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
     8	21/12/23 02:05:26 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
     9	21/12/23 02:07:19 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    10	21/12/23 02:09:47 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    11	21/12/23 02:12:33 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    12	21/12/23 02:14:30 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    13	21/12/23 02:16:47 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    14	21/12/23 02:19:54 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    15	21/12/23 02:24:35 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    16	21/12/23 02:29:53 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    17	21/12/23 02:31:59 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    18	21/12/23 02:34:22 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    19	21/12/23 02:37:57 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    20	21/12/23 02:42:19 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    21	21/12/23 02:47:27 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    22	21/12/23 02:48:33 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    23	21/12/23 02:49:39 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    24	21/12/23 02:50:56 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    25	21/12/23 02:52:29 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    26	21/12/23 02:54:14 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    27	21/12/23 02:55:42 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    28	21/12/23 02:57:19 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    29	21/12/23 02:59:12 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    30	21/12/23 03:01:35 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    31	21/12/23 03:04:15 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    32	21/12/23 03:06:13 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    33	21/12/23 03:08:40 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    34	21/12/23 03:11:33 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    35	21/12/23 03:15:46 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    36	21/12/23 03:21:37 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    37	21/12/23 03:23:46 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    38	21/12/23 03:26:45 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    39	21/12/23 03:29:29 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    40	21/12/23 03:34:23 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    41	21/12/23 03:40:19 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    42	21/12/23 03:41:22 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    43	21/12/23 03:42:31 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    44	21/12/23 03:43:46 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    45	21/12/23 03:45:22 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    46	21/12/23 03:47:09 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    47	21/12/23 03:48:38 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    48	21/12/23 03:50:20 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    49	21/12/23 03:52:13 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    50	21/12/23 03:54:39 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    51	21/12/23 03:57:23 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    52	21/12/23 03:59:25 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    53	21/12/23 04:01:46 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    54	21/12/23 04:04:49 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    55	21/12/23 04:09:02 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    56	21/12/23 04:14:45 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    57	21/12/23 04:16:44 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    58	21/12/23 04:19:35 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    59	21/12/23 04:22:29 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    60	21/12/23 04:26:36 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    61	21/12/23 04:32:03 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    62	21/12/23 04:32:46 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    63	21/12/23 04:33:31 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    64	21/12/23 04:34:21 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    65	21/12/23 04:35:18 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    66	21/12/23 04:36:18 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    67	21/12/23 04:37:05 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    68	21/12/23 04:37:54 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    69	21/12/23 04:38:48 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    70	21/12/23 04:39:50 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    71	21/12/23 04:40:55 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    72	21/12/23 04:41:51 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    73	21/12/23 04:42:48 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    74	21/12/23 04:43:50 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    75	21/12/23 04:45:02 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    76	21/12/23 04:46:21 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    77	21/12/23 04:47:25 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    78	21/12/23 04:48:30 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    79	21/12/23 04:49:44 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    80	21/12/23 04:51:08 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    81	21/12/23 04:52:43 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    82	21/12/23 04:53:55 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    83	21/12/23 04:55:09 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    84	21/12/23 04:56:34 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    85	21/12/23 04:58:11 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    86	21/12/23 05:00:05 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    87	21/12/23 05:01:21 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    88	21/12/23 05:02:39 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    89	21/12/23 05:04:07 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    90	21/12/23 05:05:49 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    91	21/12/23 05:07:47 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    92	21/12/23 05:09:18 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    93	21/12/23 05:10:53 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    94	21/12/23 05:12:37 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    95	21/12/23 05:14:39 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    96	21/12/23 05:16:59 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    97	21/12/23 05:19:00 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    98	21/12/23 05:21:11 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
    99	21/12/23 05:23:38 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
   100	21/12/23 05:26:40 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
   101	21/12/23 05:30:19 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
   102	21/12/23 05:34:16 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
   103	21/12/23 05:41:50 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
   104	21/12/23 05:50:54 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
   105	21/12/23 06:21:56 WARN NativeCodeLoader: Unable to load native-hadoop library for your platform... using builtin-java classes where applicable
"""

end_log = """
     1	21/12/23 01:56:12 INFO ShutdownHookManager: Deleting directory /tmp/spark-7b867f43-b8de-411d-9383-bc4044d6c3ac
     2	21/12/23 01:57:19 INFO ShutdownHookManager: Deleting directory /tmp/spark-5ae7a9b4-bb88-4bc8-b34f-c4619046984f
     3	21/12/23 01:58:37 INFO ShutdownHookManager: Deleting directory /tmp/spark-638c8245-7c8c-4bd1-9f2b-b56a28e4ffa9
     4	21/12/23 02:00:17 INFO ShutdownHookManager: Deleting directory /tmp/spark-f48b2668-d415-4d2d-9106-4b84fa429fdf
     5	21/12/23 02:02:11 INFO ShutdownHookManager: Deleting directory /tmp/spark-1b24b354-f0b0-400d-aff9-1f0af805059b
     6	21/12/23 02:03:41 INFO ShutdownHookManager: Deleting directory /tmp/spark-61f41834-0d75-4a0d-9abe-ebd1d273c2d9
     7	21/12/23 02:05:20 INFO ShutdownHookManager: Deleting directory /tmp/spark-41e3ede3-b9cf-43d3-806d-32fcc7a0d2e9
     8	21/12/23 02:07:13 INFO ShutdownHookManager: Deleting directory /tmp/spark-ccc76b54-32bd-4595-bbee-b3c97106543b
     9	21/12/23 02:09:41 INFO ShutdownHookManager: Deleting directory /tmp/spark-65b6f4e8-2de1-428e-8fa0-ca216a87d0c4
    10	21/12/23 02:12:27 INFO ShutdownHookManager: Deleting directory /tmp/spark-edd7ce66-16cd-4a74-aded-51eec1e16b10
    11	21/12/23 02:14:24 INFO ShutdownHookManager: Deleting directory /tmp/spark-f49ef18f-f49f-4d8f-8f58-6d6a06f08a41
    12	21/12/23 02:16:41 INFO ShutdownHookManager: Deleting directory /tmp/spark-a285a6b2-2d24-490b-844f-562363cf01a2
    13	21/12/23 02:19:48 INFO ShutdownHookManager: Deleting directory /tmp/spark-867f04ac-8fd6-4253-9f85-15e58b0b2e99
    14	21/12/23 02:24:29 INFO ShutdownHookManager: Deleting directory /tmp/spark-ba4ef689-9e83-4ca8-8850-f2e7dc3cc446
    15	21/12/23 02:29:48 INFO ShutdownHookManager: Deleting directory /tmp/spark-da117521-c32c-4ba0-b5ae-8a464c2ad81d
    16	21/12/23 02:31:52 INFO ShutdownHookManager: Deleting directory /tmp/spark-b3261ecc-e261-4815-802a-778e77cfb079
    17	21/12/23 02:34:16 INFO ShutdownHookManager: Deleting directory /tmp/spark-5484061d-6c7d-412b-ab83-580a61745990
    18	21/12/23 02:37:51 INFO ShutdownHookManager: Deleting directory /tmp/spark-bd55f513-d204-4236-9d9c-b81237b5ebff
    19	21/12/23 02:42:13 INFO ShutdownHookManager: Deleting directory /tmp/spark-d9c1b95e-755c-4e2a-835f-092e4aa56257
    20	21/12/23 02:47:21 INFO ShutdownHookManager: Deleting directory /tmp/spark-09c8bb39-8737-42f4-a4ad-9307a84260fc
    21	21/12/23 02:48:27 INFO ShutdownHookManager: Deleting directory /tmp/spark-319aa130-49c6-4123-af28-0722dd16559b
    22	21/12/23 02:49:33 INFO ShutdownHookManager: Deleting directory /tmp/spark-c0bf0e80-2832-4364-a18a-22e3d20caf3f
    23	21/12/23 02:50:50 INFO ShutdownHookManager: Deleting directory /tmp/spark-7ba66197-2a19-4d0c-99c7-557c141db300
    24	21/12/23 02:52:23 INFO ShutdownHookManager: Deleting directory /tmp/spark-8a6f21dd-4469-4610-b90c-e950df69890a
    25	21/12/23 02:54:08 INFO ShutdownHookManager: Deleting directory /tmp/spark-d92f72a5-de88-4287-9f41-8901d5b4835d
    26	21/12/23 02:55:36 INFO ShutdownHookManager: Deleting directory /tmp/spark-7a24eb81-6f7f-46ba-893f-49577a415f71
    27	21/12/23 02:57:13 INFO ShutdownHookManager: Deleting directory /tmp/spark-bbce9092-55ca-4437-afd6-d0e8ad8af359
    28	21/12/23 02:59:06 INFO ShutdownHookManager: Deleting directory /tmp/spark-3eba9214-4f8c-4f48-8d97-6f7ace08fee5
    29	21/12/23 03:01:29 INFO ShutdownHookManager: Deleting directory /tmp/spark-2b0c1b5e-8f9c-430e-9ad4-88f2262babd3
    30	21/12/23 03:04:09 INFO ShutdownHookManager: Deleting directory /tmp/spark-be5fd141-e30e-474e-ba73-6af15680f852
    31	21/12/23 03:06:07 INFO ShutdownHookManager: Deleting directory /tmp/spark-eaa79c7b-ca86-4ded-9c7b-4f50e4f39397
    32	21/12/23 03:08:34 INFO ShutdownHookManager: Deleting directory /tmp/spark-7f622e24-a473-43cf-9d83-fa2eb3d627e5
    33	21/12/23 03:11:27 INFO ShutdownHookManager: Deleting directory /tmp/spark-7ba85fe0-29e8-4dd6-b0ab-b1f8e1fa5c39
    34	21/12/23 03:15:40 INFO ShutdownHookManager: Deleting directory /tmp/spark-a8e518c3-8743-4ae5-aa71-4b9df10c1974
    35	21/12/23 03:21:31 INFO ShutdownHookManager: Deleting directory /tmp/spark-c57324e2-35d1-458b-b2fa-05a935f568ac
    36	21/12/23 03:23:39 INFO ShutdownHookManager: Deleting directory /tmp/spark-61acb8d4-29a2-468e-bd11-a34f6cad24fd
    37	21/12/23 03:26:38 INFO ShutdownHookManager: Deleting directory /tmp/spark-0563676e-5a9a-49ec-aca4-02d9dd10cb05
    38	21/12/23 03:29:23 INFO ShutdownHookManager: Deleting directory /tmp/spark-e8eff768-6c3b-490f-90d4-5676bfefd946
    39	21/12/23 03:34:17 INFO ShutdownHookManager: Deleting directory /tmp/spark-1cec09e5-13c6-46b3-a74e-e62f319e4eaa
    40	21/12/23 03:40:13 INFO ShutdownHookManager: Deleting directory /tmp/spark-40d40d11-a1ab-4d56-87f4-82ed8f04dae3
    41	21/12/23 03:41:16 INFO ShutdownHookManager: Deleting directory /tmp/spark-3184a57b-196e-4eaa-99f1-96322211338d
    42	21/12/23 03:42:25 INFO ShutdownHookManager: Deleting directory /tmp/spark-279531c5-792f-4098-944d-ebce404a7d63
    43	21/12/23 03:43:40 INFO ShutdownHookManager: Deleting directory /tmp/spark-a81e4324-fe64-4399-9105-6e4bf6691aad
    44	21/12/23 03:45:17 INFO ShutdownHookManager: Deleting directory /tmp/spark-977d5756-6dee-45f0-badb-cae8b97920f2
    45	21/12/23 03:47:03 INFO ShutdownHookManager: Deleting directory /tmp/spark-847d43c6-3880-48ce-8ebf-a9b2781d74ab
    46	21/12/23 03:48:32 INFO ShutdownHookManager: Deleting directory /tmp/spark-7d2e371c-f706-4818-aaea-9e0cdaa7ee7a
    47	21/12/23 03:50:14 INFO ShutdownHookManager: Deleting directory /tmp/spark-85e59322-75bf-4c9d-8ab7-1070c1b7952a
    48	21/12/23 03:52:07 INFO ShutdownHookManager: Deleting directory /tmp/spark-c24afe27-a0a9-4cd6-a67e-f0784315aed2
    49	21/12/23 03:54:33 INFO ShutdownHookManager: Deleting directory /tmp/spark-642287a1-2403-4198-90d0-dfd5c40b9a7a
    50	21/12/23 03:57:17 INFO ShutdownHookManager: Deleting directory /tmp/spark-4844d3cc-61da-4fac-8945-0d50a59e7016
    51	21/12/23 03:59:19 INFO ShutdownHookManager: Deleting directory /tmp/spark-b2489047-ea7e-4f77-95d0-232ae303786d
    52	21/12/23 04:01:40 INFO ShutdownHookManager: Deleting directory /tmp/spark-1b2432aa-a140-4903-8645-e00932949e16
    53	21/12/23 04:04:43 INFO ShutdownHookManager: Deleting directory /tmp/spark-8f9a13bb-ce04-4095-bb81-0d0ecb9c1fbc
    54	21/12/23 04:08:56 INFO ShutdownHookManager: Deleting directory /tmp/spark-27148a38-25b6-44ac-a9ea-de5cf74b823d
    55	21/12/23 04:14:39 INFO ShutdownHookManager: Deleting directory /tmp/spark-b384fd7d-400c-499f-8090-a930d7598f0d
    56	21/12/23 04:16:38 INFO ShutdownHookManager: Deleting directory /tmp/spark-31a4f794-0c18-4a1e-a144-4ca0314d976f
    57	21/12/23 04:19:29 INFO ShutdownHookManager: Deleting directory /tmp/spark-87de1658-c518-4ef8-9b0c-3c3f2bfe0fbc
    58	21/12/23 04:22:23 INFO ShutdownHookManager: Deleting directory /tmp/spark-a1f5bd7b-23b0-4e67-97ba-23870794cd66
    59	21/12/23 04:26:23 INFO ShutdownHookManager: Deleting directory /tmp/spark-4d90190e-07a8-4bc0-a8cc-e3d2f722437b
    60	21/12/23 04:31:57 INFO ShutdownHookManager: Deleting directory /tmp/spark-0def10a4-8a03-4788-90aa-c02499d0b591
    61	21/12/23 04:32:40 INFO ShutdownHookManager: Deleting directory /tmp/spark-024011c9-2662-47a5-9a87-2e7e00f57e6b
    62	21/12/23 04:33:25 INFO ShutdownHookManager: Deleting directory /tmp/spark-0f6ce0d8-a79a-4933-96a2-bf960ef8c4ac
    63	21/12/23 04:34:15 INFO ShutdownHookManager: Deleting directory /tmp/spark-e7a68d1f-12a7-4374-b93a-ff4dd3abb229
    64	21/12/23 04:35:12 INFO ShutdownHookManager: Deleting directory /tmp/spark-940eca2f-4625-4536-88c2-2bf9c78e0843
    65	21/12/23 04:36:12 INFO ShutdownHookManager: Deleting directory /tmp/spark-039faab1-959c-42ad-8214-e1a959d1a37f
    66	21/12/23 04:36:59 INFO ShutdownHookManager: Deleting directory /tmp/spark-05797628-588d-4d3a-a219-661507dbb33b
    67	21/12/23 04:37:48 INFO ShutdownHookManager: Deleting directory /tmp/spark-5992f261-3bd9-4342-840a-55fab03f3e49
    68	21/12/23 04:38:42 INFO ShutdownHookManager: Deleting directory /tmp/spark-17e9604c-091a-4cb8-a419-91cabb113550
    69	21/12/23 04:39:44 INFO ShutdownHookManager: Deleting directory /tmp/spark-3138552e-5a01-4966-bbff-97655127f787
    70	21/12/23 04:40:49 INFO ShutdownHookManager: Deleting directory /tmp/spark-6bcfeff1-4dc0-4752-b634-a7c2cbfe459c
    71	21/12/23 04:41:45 INFO ShutdownHookManager: Deleting directory /tmp/spark-24866c67-e761-46b8-8df8-57515125f31d
    72	21/12/23 04:42:42 INFO ShutdownHookManager: Deleting directory /tmp/spark-61f2a8b5-5857-41d9-b52a-64e1a398221a
    73	21/12/23 04:43:44 INFO ShutdownHookManager: Deleting directory /tmp/spark-77c9e551-68cf-4467-8efd-031d9566116b
    74	21/12/23 04:44:56 INFO ShutdownHookManager: Deleting directory /tmp/spark-0bdf54dc-7699-4f7b-8431-fe349946de4d
    75	21/12/23 04:46:15 INFO ShutdownHookManager: Deleting directory /tmp/spark-f925740e-da3d-41e7-bf75-42d8539fa45e
    76	21/12/23 04:47:19 INFO ShutdownHookManager: Deleting directory /tmp/spark-1966450c-b736-4d27-b34c-64ea5337367a
    77	21/12/23 04:48:24 INFO ShutdownHookManager: Deleting directory /tmp/spark-7da9d9af-2620-403d-a228-dc3e75ed9b5a
    78	21/12/23 04:49:38 INFO ShutdownHookManager: Deleting directory /tmp/spark-09d865dc-067c-49e0-ba62-2414431d009b
    79	21/12/23 04:51:02 INFO ShutdownHookManager: Deleting directory /tmp/spark-8d92ab66-e4f1-4e98-aa78-8668d201cf87
    80	21/12/23 04:52:37 INFO ShutdownHookManager: Deleting directory /tmp/spark-8749c109-03cb-47c1-90e3-85957c1ccbf5
    81	21/12/23 04:53:49 INFO ShutdownHookManager: Deleting directory /tmp/spark-d288616d-c25d-4db6-97a9-a51d16ab7a98
    82	21/12/23 04:55:04 INFO ShutdownHookManager: Deleting directory /tmp/spark-e9bb132f-7274-49c5-8eb0-c9f930fd95d2
    83	21/12/23 04:56:27 INFO ShutdownHookManager: Deleting directory /tmp/spark-f30bfb68-bdf1-403c-bfc6-834cc2318462
    84	21/12/23 04:58:05 INFO ShutdownHookManager: Deleting directory /tmp/spark-8fd75995-9b1d-46c7-9188-ad27ef6309f9
    85	21/12/23 05:00:00 INFO ShutdownHookManager: Deleting directory /tmp/spark-f29f8f6f-0429-4992-a7ad-6902868c2c13
    86	21/12/23 05:01:14 INFO ShutdownHookManager: Deleting directory /tmp/spark-079d27f0-a98a-4a0e-aa62-1ff1f1b6b6be
    87	21/12/23 05:02:33 INFO ShutdownHookManager: Deleting directory /tmp/spark-f3abd30a-65e7-495f-b334-2acc7a5a5ab7
    88	21/12/23 05:04:01 INFO ShutdownHookManager: Deleting directory /tmp/spark-a4f40e9d-6fe3-4577-9aaa-3e7538dd0d0b
    89	21/12/23 05:05:43 INFO ShutdownHookManager: Deleting directory /tmp/spark-549fa44d-f74e-4193-89a5-d8bd4dee22f2
    90	21/12/23 05:07:41 INFO ShutdownHookManager: Deleting directory /tmp/spark-c6ff6e29-0aea-4a32-a642-1d32d458f3a0
    91	21/12/23 05:09:12 INFO ShutdownHookManager: Deleting directory /tmp/spark-593a8861-5e9a-456a-81b3-af5dd8881a07
    92	21/12/23 05:10:47 INFO ShutdownHookManager: Deleting directory /tmp/spark-fb7a9b75-2e47-43ac-848e-11ccddc41e67
    93	21/12/23 05:12:31 INFO ShutdownHookManager: Deleting directory /tmp/spark-81b6cbf4-fdf1-4039-a289-eae1a89027be
    94	21/12/23 05:14:32 INFO ShutdownHookManager: Deleting directory /tmp/spark-6325537c-0b7a-4b0c-bec4-03ea596abc50
    95	21/12/23 05:16:52 INFO ShutdownHookManager: Deleting directory /tmp/spark-a055bc06-a229-44e0-8863-2f23328078f4
    96	21/12/23 05:18:54 INFO ShutdownHookManager: Deleting directory /tmp/spark-25d44f96-bc9d-41c4-968d-964a52b32cec
    97	21/12/23 05:21:05 INFO ShutdownHookManager: Deleting directory /tmp/spark-99fb1934-ee48-4300-8475-37b5f0ef1668
    98	21/12/23 05:23:32 INFO ShutdownHookManager: Deleting directory /tmp/spark-0af1bfd5-f344-4b69-80dc-060d006e7d09
    99	21/12/23 05:26:33 INFO ShutdownHookManager: Deleting directory /tmp/spark-af0cead4-24d9-4da4-81c1-eef1cd250b76
   100	21/12/23 05:30:13 INFO ShutdownHookManager: Deleting directory /tmp/spark-66e5f6c4-becd-4ddb-8285-b46e837475bb
   101	21/12/23 05:34:10 INFO ShutdownHookManager: Deleting directory /tmp/spark-64f56d6d-87f1-4273-a2f0-37083006462d
   102	21/12/23 05:41:43 INFO ShutdownHookManager: Deleting directory /tmp/spark-5102b58e-7a44-49c4-a7fe-ec41379e21a4
   103	21/12/23 05:50:47 INFO ShutdownHookManager: Deleting directory /tmp/spark-ee31869d-0aa2-4b80-88ec-bfd0348cef0e
   104	21/12/23 06:21:50 INFO ShutdownHookManager: Deleting directory /tmp/spark-16f1aa52-b46c-4d85-8f0b-f522b0a822d8
   105	21/12/23 07:00:24 INFO ShutdownHookManager: Deleting directory /tmp/spark-77f0e823-6246-46a8-a0e2-6560ba67dfc3
"""

commands = """
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_500000_10.out" --numEPart=-1 --numIter=1
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_500000_10.out" --numEPart=-1 --numIter=2
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_500000_10.out" --numEPart=-1 --numIter=4
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_500000_10.out" --numEPart=-1 --numIter=8
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_500000_10.out" --numEPart=-1 --numIter=12

    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_500000_100.out" --numEPart=-1 --numIter=1
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_500000_100.out" --numEPart=-1 --numIter=2
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_500000_100.out" --numEPart=-1 --numIter=4
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_500000_100.out" --numEPart=-1 --numIter=8
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_500000_100.out" --numEPart=-1 --numIter=12

    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_5000000_10.out" --numEPart=-1 --numIter=1
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_5000000_10.out" --numEPart=-1 --numIter=2
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_5000000_10.out" --numEPart=-1 --numIter=4
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_5000000_10.out" --numEPart=-1 --numIter=8
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_5000000_10.out" --numEPart=-1 --numIter=12

    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/soc-LiveJournal1.out" --numEPart=-1 --numIter=1
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/soc-LiveJournal1.out" --numEPart=-1 --numIter=2
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/soc-LiveJournal1.out" --numEPart=-1 --numIter=4
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/soc-LiveJournal1.out" --numEPart=-1 --numIter=8
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/soc-LiveJournal1.out" --numEPart=-1 --numIter=12

    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_500000_10.out" --numEPart=-1 --numIter=1
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_500000_10.out" --numEPart=-1 --numIter=2
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_500000_10.out" --numEPart=-1 --numIter=4
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_500000_10.out" --numEPart=-1 --numIter=8
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_500000_10.out" --numEPart=-1 --numIter=12

    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_500000_100.out" --numEPart=-1 --numIter=1
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_500000_100.out" --numEPart=-1 --numIter=2
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_500000_100.out" --numEPart=-1 --numIter=4
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_500000_100.out" --numEPart=-1 --numIter=8
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_500000_100.out" --numEPart=-1 --numIter=12

    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_5000000_10.out" --numEPart=-1 --numIter=1
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_5000000_10.out" --numEPart=-1 --numIter=2
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_5000000_10.out" --numEPart=-1 --numIter=4
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_5000000_10.out" --numEPart=-1 --numIter=8
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_5000000_10.out" --numEPart=-1 --numIter=12

    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/soc-LiveJournal1.out" --numEPart=-1 --numIter=1
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/soc-LiveJournal1.out" --numEPart=-1 --numIter=2
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/soc-LiveJournal1.out" --numEPart=-1 --numIter=4
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/soc-LiveJournal1.out" --numEPart=-1 --numIter=8
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/soc-LiveJournal1.out" --numEPart=-1 --numIter=12

    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_500000_10.out" --numEPart=-1 --numIter=1
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_500000_10.out" --numEPart=-1 --numIter=2
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_500000_10.out" --numEPart=-1 --numIter=4
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_500000_10.out" --numEPart=-1 --numIter=8
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_500000_10.out" --numEPart=-1 --numIter=12

    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_500000_100.out" --numEPart=-1 --numIter=1
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_500000_100.out" --numEPart=-1 --numIter=2
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_500000_100.out" --numEPart=-1 --numIter=4
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_500000_100.out" --numEPart=-1 --numIter=8
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_500000_100.out" --numEPart=-1 --numIter=12

    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_5000000_10.out" --numEPart=-1 --numIter=1
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_5000000_10.out" --numEPart=-1 --numIter=2
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_5000000_10.out" --numEPart=-1 --numIter=4
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_5000000_10.out" --numEPart=-1 --numIter=8
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_5000000_10.out" --numEPart=-1 --numIter=12

    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/soc-LiveJournal1.out" --numEPart=-1 --numIter=1
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/soc-LiveJournal1.out" --numEPart=-1 --numIter=2
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/soc-LiveJournal1.out" --numEPart=-1 --numIter=4
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/soc-LiveJournal1.out" --numEPart=-1 --numIter=8
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/soc-LiveJournal1.out" --numEPart=-1 --numIter=12

    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_100000_10.out" --numEPart=-1 --numIter=1
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_100000_10.out" --numEPart=-1 --numIter=2
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_100000_10.out" --numEPart=-1 --numIter=4
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_100000_10.out" --numEPart=-1 --numIter=8
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_100000_10.out" --numEPart=-1 --numIter=12

    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_100000_20.out" --numEPart=-1 --numIter=1
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_100000_20.out" --numEPart=-1 --numIter=2
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_100000_20.out" --numEPart=-1 --numIter=4
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_100000_20.out" --numEPart=-1 --numIter=8
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_100000_20.out" --numEPart=-1 --numIter=12

    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_100000_40.out" --numEPart=-1 --numIter=1
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_100000_40.out" --numEPart=-1 --numIter=2
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_100000_40.out" --numEPart=-1 --numIter=4
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_100000_40.out" --numEPart=-1 --numIter=8
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_100000_40.out" --numEPart=-1 --numIter=12

    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_100000_80.out" --numEPart=-1 --numIter=1
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_100000_80.out" --numEPart=-1 --numIter=2
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_100000_80.out" --numEPart=-1 --numIter=4
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_100000_80.out" --numEPart=-1 --numIter=8
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_100000_80.out" --numEPart=-1 --numIter=12

    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_100000_160.out" --numEPart=-1 --numIter=1
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_100000_160.out" --numEPart=-1 --numIter=2
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_100000_160.out" --numEPart=-1 --numIter=4
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_100000_160.out" --numEPart=-1 --numIter=8
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_100000_160.out" --numEPart=-1 --numIter=12

    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_100000_320.out" --numEPart=-1 --numIter=1
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_100000_320.out" --numEPart=-1 --numIter=2
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_100000_320.out" --numEPart=-1 --numIter=4
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_100000_320.out" --numEPart=-1 --numIter=8
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_100000_320.out" --numEPart=-1 --numIter=12

    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_100000_640.out" --numEPart=-1 --numIter=1
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_100000_640.out" --numEPart=-1 --numIter=2
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_100000_640.out" --numEPart=-1 --numIter=4
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_100000_640.out" --numEPart=-1 --numIter=8
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_100000_640.out" --numEPart=-1 --numIter=12

    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_100000_1280.out" --numEPart=-1 --numIter=1
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_100000_1280.out" --numEPart=-1 --numIter=2
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_100000_1280.out" --numEPart=-1 --numIter=4
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_100000_1280.out" --numEPart=-1 --numIter=8
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_100000_1280.out" --numEPart=-1 --numIter=12

    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_100000_2560.out" --numEPart=-1 --numIter=1
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_100000_2560.out" --numEPart=-1 --numIter=2
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_100000_2560.out" --numEPart=-1 --numIter=4
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_100000_2560.out" --numEPart=-1 --numIter=8
    time ~/spark-3.1.2-bin-hadoop3.2/bin/run-example --master spark://wca-rpi4-7:7077 --executor-memory 2500m graphx.Analytics pagerank "/home/wcauser3/thesis_page_rank/cpp/page_rank_with_redis/data/graph_100000_2560.out" --numEPart=-1 --numIter=12
"""

start_times = []
end_times = []
files = []
round_counts = []
runtimes = []

for line in start_log.splitlines():
    line = line.strip()
    if not len(line): continue
    time_str = re.findall(r"(\d\d/\d\d/\d\d \d\d:\d\d:\d\d)", line)[0]
    time_obj = datetime.strptime(time_str, '%y/%m/%d %H:%M:%S')
    start_times.append(time_obj)
    # print(time_obj)

for line in end_log.splitlines():
    line = line.strip()
    if not len(line): continue
    time_str = re.findall(r"(\d\d/\d\d/\d\d \d\d:\d\d:\d\d)", line)[0]
    time_obj = datetime.strptime(time_str, '%y/%m/%d %H:%M:%S')
    end_times.append(time_obj)
    # print(time_obj)

for line in commands.splitlines():
    line = line.strip()
    if not len(line): continue
    file = re.findall(r"data/(.+)\.out", line)[0]
    round_count = int(re.findall(r"--numIter=(\d+)", line)[0])
    files.append(file)
    round_counts.append(round_count)
    # print(file, round_count)

for i in range( len(start_times) ):
    runtime = (end_times[i] - start_times[i]).total_seconds()
    runtimes.append(runtime)
    diff_time = 0
    diff_time_per_round = 0
    if( round_counts[i] != 1 ):
        diff_time = runtime - runtimes[i-1]
        diff_time_per_round = diff_time / ( round_counts[i] - round_counts[i-1] )
    else:
        print("---------------------------------------")
    # exp_result = f"{files[i]} {round_counts[i]} {runtime} {diff_time} {diff_time_per_round}"
    exp_result = f"{files[i]} {diff_time_per_round}"
    print(exp_result)


# ---------------------------------------
# graph_500000_10 0
# graph_500000_10 3.0
# graph_500000_10 6.0
# graph_500000_10 5.25
# graph_500000_10 3.75
# ---------------------------------------
# graph_500000_100 0
# graph_500000_100 8.0
# graph_500000_100 7.5
# graph_500000_100 8.75
# graph_500000_100 4.5
# ---------------------------------------
# graph_5000000_10 0
# graph_5000000_10 20.0
# graph_5000000_10 25.0
# graph_5000000_10 23.5
# graph_5000000_10 9.5
# ---------------------------------------
# soc-LiveJournal1 0
# soc-LiveJournal1 18.0
# soc-LiveJournal1 36.0
# soc-LiveJournal1 11.75
# soc-LiveJournal1 11.5
# ---------------------------------------
# graph_500000_10 0
# graph_500000_10 0.0
# graph_500000_10 5.5
# graph_500000_10 4.0
# graph_500000_10 3.0
# ---------------------------------------
# graph_500000_100 0
# graph_500000_100 9.0
# graph_500000_100 8.0
# graph_500000_100 7.5
# graph_500000_100 4.25
# ---------------------------------------
# graph_5000000_10 0
# graph_5000000_10 29.0
# graph_5000000_10 13.0
# graph_5000000_10 20.0
# graph_5000000_10 24.5
# ---------------------------------------
# soc-LiveJournal1 0
# soc-LiveJournal1 50.0
# soc-LiveJournal1 -7.0
# soc-LiveJournal1 32.5
# soc-LiveJournal1 15.5
# ---------------------------------------
# graph_500000_10 0
# graph_500000_10 6.0
# graph_500000_10 3.0
# graph_500000_10 5.5
# graph_500000_10 2.5
# ---------------------------------------
# graph_500000_100 0
# graph_500000_100 13.0
# graph_500000_100 5.5
# graph_500000_100 8.25
# graph_500000_100 4.5
# ---------------------------------------
# graph_5000000_10 0
# graph_5000000_10 19.0
# graph_5000000_10 21.0
# graph_5000000_10 17.5
# graph_5000000_10 22.5
# ---------------------------------------
# soc-LiveJournal1 0
# soc-LiveJournal1 52.0
# soc-LiveJournal1 1.5
# soc-LiveJournal1 16.5
# soc-LiveJournal1 21.75
# ---------------------------------------
# graph_100000_10 0
# graph_100000_10 2.0
# graph_100000_10 2.5
# graph_100000_10 1.75
# graph_100000_10 0.75
# ---------------------------------------
# graph_100000_20 0
# graph_100000_20 2.0
# graph_100000_20 2.5
# graph_100000_20 2.0
# graph_100000_20 0.75
# ---------------------------------------
# graph_100000_40 0
# graph_100000_40 1.0
# graph_100000_40 2.5
# graph_100000_40 2.5
# graph_100000_40 1.75
# ---------------------------------------
# graph_100000_80 0
# graph_100000_80 1.0
# graph_100000_80 4.5
# graph_100000_80 2.5
# graph_100000_80 2.75
# ---------------------------------------
# graph_100000_160 0
# graph_100000_160 3.0
# graph_100000_160 4.5
# graph_100000_160 3.25
# graph_100000_160 4.5
# ---------------------------------------
# graph_100000_320 0
# graph_100000_320 3.0
# graph_100000_320 5.0
# graph_100000_320 3.5
# graph_100000_320 4.0
# ---------------------------------------
# graph_100000_640 0
# graph_100000_640 4.0
# graph_100000_640 4.5
# graph_100000_640 4.25
# graph_100000_640 4.5
# ---------------------------------------
# graph_100000_1280 0
# graph_100000_1280 10.0
# graph_100000_1280 8.0
# graph_100000_1280 8.5
# graph_100000_1280 9.5
# ---------------------------------------
# graph_100000_2560 0
# graph_100000_2560 216.0
# graph_100000_2560 45.0
# graph_100000_2560 329.75
# graph_100000_2560 113.0