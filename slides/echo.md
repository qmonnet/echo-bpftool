---
title: eCHO #11 - bpftool
author: Quentin Monnet
date: 2021-06-25
header-includes:
  - \usepackage{FiraSans}
  - \usepackage[scale=0.85]{FiraMono}
  - \setbeamertemplate{description item}{\small\color{accent}\insertdescriptionitem}
  - \makeatletter\xpatchcmd{\@@description}{\raggedright}{\raggedright\footnotesize\selectfont}{}{}\makeatother
  - \renewcommand{\caption}{}
  - \makeatletter\setkeys{beamer@margin}{description width=2em}\makeatother
  - \setbeamercovered{invisible}
  - \newcommand{\blap}[1]{\mbox{\vbox to 0pt{\hbox{#1}\vss}}}
  - \newcommand{\tlap}[1]{\mbox{\vbox to 0pt{\vss\hbox{#1}}}}
fontsize: 12pt
---

# A few notes on the demo

- 4 parts:

    - Simple introspection
    - Attach to XDP
    - BTF debug information
    - Level up: more complex commands

- Code and commands available at <https://github.com/qmonnet/echo-bpftool>

- Latest kernel, clang/LLVM, bpftool

- Weird typing: ZSH autosuggestions & colours

# About bpftool

- Sources: Linux kernel repository

- Packaged for some distributions (Ubuntu, Fedora, ...)

- Man pages at <https://www.mankier.com/package/bpftool>

- Interactive help:

    - `bpftool help`
    - `bpftool prog help`
    - ...

---

\includegraphics[height=\paperheight]{loading.pdf}

# Use cases for bpftool in Cilium

- Probe the host:
    - Does it validate the requirements for Cilium?
    - ... for the desired features?

- Load some programs \
  (socket operations, `bpf_sock.o`)

- Gather data for sysdumps
