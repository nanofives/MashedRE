<!-- CROSS-AREA FINDING BUS -->
<!-- Star topology: children report cross-area findings to the PARENT orchestrator.     -->
<!-- The parent (and ONLY the parent) appends rows below the ENTRIES marker and pings    -->
<!-- the affected child via mcp__happy__send_to_session. A child NEVER edits this file    -->
<!-- directly; it messages the parent, which arbitrates and records.                     -->
<!--                                                                                      -->
<!-- This is the DURABLE channel: a finding survives here whether or not the affected     -->
<!-- session is live. The send_to_session ping is the LIVE channel (steering only); if    -->
<!-- the peer is idle/closed it will read the row on its next wake instead.               -->
<!--                                                                                      -->
<!-- Row schema (one row per finding, pipe-delimited):                                    -->
<!--   id | date | from-area | affects-area | kind | anchor | claim | status | resolution -->
<!--     id          B-#### monotonic, never reused                                       -->
<!--     from-area   canonical subsystem that discovered it                               -->
<!--     affects-area canonical subsystem(s) that must react (comma-sep)                  -->
<!--     kind        struct-offset | global | shared-rva | dispatcher | format | assumption-->
<!--     anchor      RVA / struct+offset / global addr the claim is about (cite, per NO-GUESSING)-->
<!--     claim       one line, mechanical, no intent words                                -->
<!--     status      OPEN | PINGED | ACK | LANDED | REJECTED                              -->
<!--     resolution  filled when status leaves OPEN: what the affected area did           -->
<!--                                                                                      -->
<!-- Rule: a struct/global/shared-rva finding is ALSO a merge fact. Once the scribe (or   -->
<!-- the header edit that documents it) lands it, both areas read the same value; the bus -->
<!-- row moves to LANDED and cites where it landed. Do not carry a LANDED fact as OPEN.   -->

# Cross-area finding bus

Next id: **B-0001**

## Active

<!-- ENTRIES -->

## Resolved

<!-- RESOLVED -->
