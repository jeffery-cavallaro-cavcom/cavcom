# Python

This is the Python support library.  The code in the contained packages adheres
to the following guidelines:

1. All code passes pylint, albeit with some disables in the code.

1. Unit tests are coded using unittest and strive for 100% coverage.

1. Where applicable, multithreaded and single event loop (i.e., asyncio) models
are supported.

1. In line with efforts to retire the GIL, all code is written without GIL  synchronization assumptions.  Thus, explicit locks (nutexes) are used where appropriate.

1. Multithreaded code is assumed to be thread-safe; however, asyncio code is
assumed to be *not* thread-safe.
