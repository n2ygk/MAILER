# Reconstructed MAILER sources

This orphan branch was generated from `mailer-base` and the CMS UPDATE/AUX decks in `mailer-updates`. CMS sequence columns 73-80 were used for reconstruction and removed from the readable files under `src/`. AUX entries were applied bottom-to-top.

## Chains not reconstructed

These chains lacked an applicable original base, were optional, or conflicted with the active control-file chain:

- `mailer-updates/ccomp.auxexcr2: no applicable sequenced base`
- `mailer-updates/install.auxr2: no applicable sequenced base (install.$script: mailer-updates/install.r205001: sequence 00089910 not found)`
- `mailer-updates/mailer.auxexcr2: no applicable sequenced base`
- `mailer-updates/mailexec.auxexcr2: no applicable sequenced base`
- `mailer-updates/mailgen.auxexcr2: no applicable sequenced base`
- `mailer-updates/mailmod1.auxexcr2: no applicable sequenced base`
- `mailer-updates/mailmod2.auxexcr2: no applicable sequenced base`
- `mailer-updates/forward.auxr2: alternate chain for forward.mailer; using forward.r207001`

## Snapshot-only files

The following readable final files are present, but their earlier commits cannot be reconstructed because the required sequenced base is absent. They are added in the final archival commit: `src/ccomp.exec`, `src/mailer.exec`, `src/mailexec.exec`, `src/mailgen.exec`, `src/mailmod1.exec`, `src/mailmod2.exec`.
