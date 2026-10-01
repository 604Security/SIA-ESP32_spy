// SIA (Secret Intelligence Agency) web app, served at "/". Plain HTML/CSS/JS, no build step.
// The mascot pixel art between ART-BEGIN / ART-END is generated: python3 spy_art.py page
#pragma once

const char PAGE_HTML[] PROGMEM = R"rawliteral(<!doctype html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>SIA · Secret Intelligence Agency</title>
<link rel="preconnect" href="https://fonts.googleapis.com">
<link href="https://fonts.googleapis.com/css2?family=Fredoka:wght@400;600;700&family=Special+Elite&family=VT323&family=Share+Tech+Mono&family=Orbitron:wght@600&display=swap" rel="stylesheet">
<style>
:root{
  color-scheme:dark;
  --bg:#080C12; --grid:rgba(120,160,200,.06); --panel:#0F1721; --panel2:#152030; --line:#27384D; --ink:#EAF2FB; --muted:#93A7BC;
  --paper:#EADCB4; --paper2:#E0CF9F; --paper-ink:#2B2416; --stamp:#D92D3C;
  --term:#39FF9E; --term-dim:#1D7A52; --term-bg:#04130C; --amber:#FFC940; --red:#FF4B5C;
  --accent:#39FF9E; --accent2:#37C6FF; --accent-ink:#03140B;
  --type:"Special Elite","Courier New",monospace; --mono:"VT323",ui-monospace,Menlo,monospace;
  --round:"Fredoka","Trebuchet MS",system-ui,sans-serif;
}
body[data-agent="megaspy"]{ --accent:#FF5FA2; --accent2:#B98CFF; --accent-ink:#2A0616; }
*{box-sizing:border-box}
body{margin:0; background-color:var(--bg); color:var(--ink); font-family:var(--round); font-size:18px;
  background-image:linear-gradient(var(--grid) 1px,transparent 1px),linear-gradient(90deg,var(--grid) 1px,transparent 1px),
    radial-gradient(ellipse at top,rgba(55,198,255,.08),transparent 60%);
  background-size:32px 32px,32px 32px,100% 100%; min-height:100vh}
body[data-agent="megaspy"]{background-image:linear-gradient(var(--grid) 1px,transparent 1px),linear-gradient(90deg,var(--grid) 1px,transparent 1px),
    radial-gradient(ellipse at top,rgba(255,95,162,.14),transparent 60%)}
main{max-width:900px; margin:0 auto; padding:0 16px 60px}
a{color:inherit}
h1,h2,h3{font-family:var(--type); font-weight:400; letter-spacing:.5px}
h2{font-size:32px; margin:14px 0 6px}
.hint{color:var(--muted); font-size:19px; margin:4px 0 16px}
[hidden]{display:none!important}

/* ---------- header ---------- */
header{display:flex; align-items:center; justify-content:space-between; gap:14px; max-width:900px; margin:0 auto; padding:16px 16px 8px; flex-wrap:wrap}
.brand{display:flex; align-items:center; gap:14px; text-decoration:none}
.logo{font-family:ui-monospace,Menlo,Consolas,monospace; font-size:clamp(9px,1.9vw,12px); line-height:1.05; margin:0; color:var(--accent);
  text-shadow:0 0 12px color-mix(in srgb,var(--accent) 45%,transparent)}
.brand .name{font-family:var(--type); font-size:15px; color:var(--muted); line-height:1.3}
.brand .name b{display:block; color:var(--ink); font-size:20px; font-weight:400}
.chip{display:flex; align-items:center; gap:10px; padding:6px 14px 6px 8px; border:2px solid var(--accent); border-radius:999px;
  background:var(--panel); text-decoration:none; box-shadow:0 0 16px color-mix(in srgb,var(--accent) 30%,transparent)}
.chip canvas{width:34px; height:38px; image-rendering:pixelated}
.chip b{display:block; font-size:18px}
.chip small{color:var(--muted); font-size:14px}

/* ---------- common bits ---------- */
.back{display:inline-block; margin:12px 0 2px; padding:6px 14px; border:2px solid var(--line); border-radius:999px; background:var(--panel);
  text-decoration:none; font-weight:600; color:var(--muted)}
.back:hover{color:var(--ink); border-color:var(--accent)}
button,.btn{font:inherit; font-weight:700; font-size:21px; cursor:pointer; color:var(--accent-ink); background:var(--accent);
  border:0; border-radius:16px; padding:12px 22px; box-shadow:0 5px 0 color-mix(in srgb,var(--accent) 55%,#000); text-decoration:none;
  display:inline-flex; align-items:center; gap:8px; transition:transform .08s, box-shadow .08s}
button:hover,.btn:hover{filter:brightness(1.08)}
button:active,.btn:active{transform:translateY(4px); box-shadow:0 1px 0 color-mix(in srgb,var(--accent) 55%,#000)}
button.ghost{background:var(--panel2); color:var(--ink); box-shadow:0 5px 0 #05080C; border:2px solid var(--line)}
button:disabled{opacity:.45; cursor:default}
.row{display:flex; flex-wrap:wrap; gap:12px; align-items:center; margin:14px 0}
input[type=text]{font:inherit; font-size:24px; padding:10px 14px; border-radius:14px; border:2px solid var(--line); background:var(--term-bg);
  color:var(--term); font-family:var(--mono); letter-spacing:2px; width:min(100%,320px); text-transform:uppercase}
input[type=text]:focus{outline:none; border-color:var(--accent)}
input[type=range]{width:min(100%,360px); accent-color:var(--accent)}

/* the manila case-file look */
.dossier{position:relative; background:linear-gradient(180deg,var(--paper),var(--paper2)); color:var(--paper-ink); border-radius:6px 18px 18px 18px;
  padding:20px 22px; font-family:var(--type); box-shadow:0 10px 26px rgba(0,0,0,.45); margin-top:22px}
.dossier::before{content:""; position:absolute; top:-16px; left:0; width:150px; height:18px; background:var(--paper); border-radius:10px 10px 0 0}
.dossier h3{margin:0 0 8px; font-size:26px}
.dossier p{font-size:19px; line-height:1.5; margin:8px 0}
.stamp{position:absolute; top:14px; right:16px; transform:rotate(8deg); border:3px solid var(--stamp); color:var(--stamp); padding:2px 10px;
  font-family:var(--type); font-size:17px; letter-spacing:2px; border-radius:6px; opacity:.85}

/* the green terminal look for gadget screens */
.screen{background:var(--term-bg); border:2px solid var(--term-dim); border-radius:16px; padding:16px 18px; color:var(--term); font-family:var(--mono);
  font-size:26px; line-height:1.2; box-shadow:inset 0 0 30px rgba(57,255,158,.08), 0 0 20px rgba(57,255,158,.08); position:relative; overflow:hidden; margin-top:16px}
.screen::after{content:""; position:absolute; inset:0; pointer-events:none;
  background:repeating-linear-gradient(0deg,rgba(0,0,0,.18) 0 2px,transparent 2px 4px)}
.screen .big{font-size:56px; letter-spacing:6px}
.cursor::after{content:"▮"; animation:blink 1s steps(1) infinite}
@keyframes blink{50%{opacity:0}}

/* ---------- login ---------- */
.agents{display:grid; grid-template-columns:repeat(auto-fit,minmax(260px,1fr)); gap:20px; margin-top:12px}
.agent-card{background:var(--panel); border:3px solid var(--line); border-radius:24px; padding:18px; text-align:center; cursor:pointer; text-decoration:none;
  transition:transform .12s, border-color .12s, box-shadow .12s}
.agent-card:hover{transform:translateY(-4px)}
.agent-card.megaspy{--c:#FF5FA2} .agent-card.spyhunter{--c:#39FF9E}
.agent-card:hover,.agent-card:focus-visible{border-color:var(--c); box-shadow:0 0 30px color-mix(in srgb,var(--c) 35%,transparent); outline:none}
.agent-card canvas{width:144px; height:160px; image-rendering:pixelated}
.agent-card b{display:block; font-family:var(--type); font-size:28px; color:var(--c); margin-top:6px}
.agent-card .rank{color:var(--muted)}
details.explain{margin-top:26px; background:var(--panel); border:2px dashed var(--line); border-radius:16px; padding:12px 16px}
details.explain summary{cursor:pointer; font-weight:700; font-size:20px}

/* ---------- HQ ---------- */
.file{display:grid; grid-template-columns:auto 1fr; gap:18px; align-items:center}
.file canvas{width:118px; height:131px; image-rendering:pixelated; background:rgba(0,0,0,.08); border-radius:10px}
.file .pts{font-family:var(--round); font-size:44px; font-weight:700; line-height:1}
.progress{height:22px; border-radius:999px; background:rgba(43,36,22,.18); overflow:hidden; margin:8px 0 4px; border:2px solid rgba(43,36,22,.35)}
.progress i{display:block; height:100%; background:var(--paper-ink); border-radius:999px; transition:width .4s}
.badges{display:flex; gap:10px; flex-wrap:wrap; margin-top:12px}
.badge{display:flex; align-items:center; gap:6px; background:rgba(43,36,22,.12); border-radius:999px; padding:4px 12px 4px 8px; font-size:16px; font-family:var(--round)}
.badge.locked{opacity:.35; filter:grayscale(1)}
.nav{display:grid; grid-template-columns:repeat(auto-fit,minmax(190px,1fr)); gap:16px; margin-top:24px}
.nav a{display:flex; flex-direction:column; gap:4px; padding:18px; border-radius:20px; background:var(--panel); border:2px solid var(--line); text-decoration:none;
  transition:transform .1s, border-color .1s}
.nav a:hover{transform:translateY(-3px); border-color:var(--accent)}
.nav .ico{font-size:40px} .nav b{font-size:24px} .nav small{color:var(--muted); font-size:16px}
.board{margin-top:26px; background:var(--panel); border:2px solid var(--line); border-radius:20px; padding:16px 18px}
.board h3{margin:0 0 10px; font-size:22px}
.lb{display:grid; grid-template-columns:130px 1fr 70px; gap:10px; align-items:center; margin:8px 0}
.lb .bar{height:24px; background:var(--panel2); border-radius:6px; overflow:hidden}
.lb .bar i{display:block; height:100%; border-radius:0 6px 6px 0; min-width:4px; transition:width .4s}
.lb .v{text-align:right; font-variant-numeric:tabular-nums; font-weight:700}
.status{color:var(--muted); font-size:15px; text-align:center; margin-top:26px}

/* ---------- missions & gadgets ---------- */
.files{display:grid; grid-template-columns:repeat(auto-fit,minmax(250px,1fr)); gap:22px 18px}
.files a{text-decoration:none}
.files .dossier{margin-top:16px; min-height:150px; transition:transform .12s}
.files a:hover .dossier{transform:rotate(-1deg) translateY(-3px)}
.files .ico{font-size:34px}
.files .meta{font-family:var(--round); font-size:15px; opacity:.8}
.tools{display:grid; grid-template-columns:repeat(auto-fit,minmax(200px,1fr)); gap:16px}
.tools a{text-decoration:none}
.tools .screen{margin:0; font-size:24px; min-height:128px; transition:transform .1s}
.tools a:hover .screen{transform:translateY(-3px); border-color:var(--term)}
.tools .ico{font-size:34px}
.tools small{display:block; color:var(--term-dim); font-size:20px}

.morse{display:grid; grid-template-columns:repeat(auto-fill,minmax(92px,1fr)); gap:6px; font-family:var(--mono); font-size:24px; margin-top:12px}
.morse div{background:var(--panel2); border-radius:8px; padding:2px 8px; display:flex; justify-content:space-between}
.morse div b{color:var(--accent)}
.slots{display:flex; gap:10px; margin:10px 0}
.slots span{width:46px; height:56px; border-bottom:4px solid var(--term); display:inline-block}
.ring{--p:0; width:200px; height:200px; border-radius:50%; display:grid; place-items:center; margin:10px auto;
  background:conic-gradient(var(--accent) calc(var(--p)*1%), var(--panel2) 0)}
.ring div{width:160px; height:160px; border-radius:50%; background:var(--bg); display:grid; place-items:center; font-family:var(--mono); font-size:64px}
.meter{height:30px; border-radius:999px; background:var(--panel2); border:2px solid var(--line); overflow:hidden; position:relative; margin-top:10px}
.meter i{display:block; height:100%; width:0; background:var(--term); transition:width .1s linear}
.meter .thr{position:absolute; top:-2px; bottom:-2px; width:4px; background:var(--red); border-radius:2px}
.wheel{font-family:var(--mono); font-size:26px; overflow-x:auto; white-space:nowrap; padding-bottom:6px}
.wheel div span{display:inline-block; width:26px; text-align:center}
.wheel .top{color:var(--muted)} .wheel .bot{color:var(--term)}
.cam{background:#000; border:3px solid var(--term-dim); border-radius:18px; overflow:hidden; max-width:640px; position:relative}
.cam img{display:block; width:100%; aspect-ratio:4/3; object-fit:cover; background:#050505}
.cam img.flip{transform:rotate(180deg)}
.cam .rec{position:absolute; top:10px; left:12px; font-family:var(--mono); font-size:24px; color:var(--red)}
.cam .rec::before{content:"● "; animation:blink 1s steps(1) infinite}
.target{font-family:var(--type); font-size:34px; color:var(--amber); margin:6px 0}
.photos{display:grid; grid-template-columns:repeat(auto-fill,minmax(150px,1fr)); gap:10px; margin-top:12px}
.photos a{display:block; border-radius:10px; overflow:hidden; border:2px solid var(--line); background:#000; position:relative}
.photos img{display:block; width:100%; aspect-ratio:4/3; object-fit:cover}
.photos a span{position:absolute; left:6px; bottom:4px; font-family:var(--mono); font-size:18px; color:var(--amber); text-shadow:0 1px 3px #000}
.log{font-family:var(--type); font-size:16px; line-height:1.6; margin:0; white-space:pre-wrap}
.trapstate{font-size:48px; letter-spacing:4px}
.trapstate.armed{color:var(--red); text-shadow:0 0 18px rgba(255,75,92,.7)}

/* ---------- celebrations & alarms ---------- */
.overlay{position:fixed; inset:0; display:grid; place-items:center; z-index:50; pointer-events:none}
.reward{background:var(--panel); border:4px solid var(--accent); border-radius:28px; padding:26px 40px; text-align:center;
  box-shadow:0 0 60px color-mix(in srgb,var(--accent) 50%,transparent); animation:pop .35s ease-out}
.reward b{display:block; font-size:64px; color:var(--accent)}
.reward span{font-size:22px}
@keyframes pop{from{transform:scale(.5); opacity:0}}
.confetti{position:fixed; top:-40px; font-size:30px; z-index:49; pointer-events:none; animation:fall linear forwards}
@keyframes fall{to{transform:translateY(110vh) rotate(540deg)}}
.alarm{position:fixed; inset:0; z-index:40; pointer-events:none; border:10px solid var(--red); animation:alarm .5s steps(1) infinite}
@keyframes alarm{50%{border-color:transparent; background:rgba(255,75,92,.08)}}
.alarm-banner{position:fixed; top:12px; left:50%; transform:translateX(-50%); z-index:41; background:var(--red); color:#fff; font-weight:700;
  font-size:24px; padding:10px 22px; border-radius:999px; box-shadow:0 6px 30px rgba(255,75,92,.6)}
/* ---------- night-vision screen: HQ (design pick #2) and the fingerprint login (pick #1) ---------- */
body{--nv:#8CFFB0; --nv-glow:rgba(140,255,176,.7); --nv1:#0F4A24; --nv2:#052210; --nv3:#010803}
body[data-agent="megaspy"]{--nv:#FFB3D6; --nv-glow:rgba(255,120,190,.7); --nv1:#4A0F33; --nv2:#22051A; --nv3:#080106}
.nv{position:relative; border-radius:20px; overflow:hidden; isolation:isolate; color:var(--nv); font-family:"Share Tech Mono",var(--mono);
  text-shadow:0 0 6px var(--nv-glow); background:radial-gradient(ellipse at center,var(--nv1) 0%,var(--nv2) 60%,var(--nv3) 100%);
  padding:56px 26px 26px; margin-top:12px; min-height:520px}
.nv::before{content:""; position:absolute; inset:0; background:repeating-linear-gradient(0deg,rgba(0,0,0,.25) 0 2px,transparent 2px 4px); z-index:5; pointer-events:none}
.nv::after{content:""; position:absolute; inset:-50%; z-index:6; pointer-events:none; opacity:.1;
  background-image:radial-gradient(#fff .7px,transparent .8px); background-size:5px 5px; animation:nvnoise .25s steps(3) infinite}
@keyframes nvnoise{to{transform:translate(3px,-2px)}}
.nv > *{position:relative; z-index:2}
.nv .cn{position:absolute; width:34px; height:34px; border:2px solid currentColor; z-index:3}
.nv .c1{top:14px; left:14px; border-right:0; border-bottom:0} .nv .c2{top:14px; right:14px; border-left:0; border-bottom:0}
.nv .c3{bottom:14px; left:14px; border-right:0; border-top:0} .nv .c4{bottom:14px; right:14px; border-left:0; border-top:0}
.nv .rec{position:absolute; top:22px; left:58px; font-size:17px} .nv .rec b{color:#FF4B5C; text-shadow:0 0 8px #FF4B5C; animation:blink 1s steps(1) infinite}
.nv .clock{position:absolute; top:22px; right:58px; font-size:17px}
.scope{position:relative; width:220px; height:220px; margin:10px auto 6px; display:grid; place-items:center}
.scope::before{content:""; position:absolute; inset:0; border:2px solid currentColor; border-radius:50%; opacity:.8}
.scope i{position:absolute; background:currentColor; opacity:.8}
.scope .h{left:-24px; right:-24px; top:50%; height:2px} .scope .v{top:-24px; bottom:-24px; left:50%; width:2px}
.scope canvas{width:118px; height:131px; image-rendering:pixelated; filter:drop-shadow(0 0 8px var(--nv-glow)); position:relative; z-index:1}
.readout{font-size:21px; line-height:1.55; margin:10px 0 4px}
.readout b{font-weight:400; letter-spacing:1px}
.nvbar{height:16px; border:2px solid currentColor; padding:2px; margin:6px 0 4px}
.nvbar i{display:block; height:100%; width:0; background:currentColor; box-shadow:0 0 10px var(--nv-glow); transition:width .5s}
.nvnext{font-size:17px; opacity:.85}
.nvbadges{display:flex; flex-wrap:wrap; gap:8px; margin:14px 0 6px; font-size:16px}
.nvbadges span{border:1.5px solid currentColor; padding:3px 8px}
.nvbadges span.locked{opacity:.3}
.nvmenu{display:grid; grid-template-columns:1fr 1fr; gap:12px; margin-top:18px}
.nvmenu a{border:2px solid currentColor; padding:14px 8px; text-align:center; font-size:20px; letter-spacing:2px; text-decoration:none; transition:background .15s, color .15s}
.nvmenu a small{display:block; font-size:14px; letter-spacing:0; opacity:.8}
.nvmenu a:hover,.nvmenu a:focus-visible{background:var(--nv); color:var(--nv3); text-shadow:none; outline:none}
/* login */
.nv.login{text-align:center}
.nvstep{font-size:18px; letter-spacing:2px; margin:6px 0 10px}
.pick{display:flex; justify-content:center; gap:16px; flex-wrap:wrap}
.pick button{all:unset; cursor:pointer; border:2px solid currentColor; padding:10px 14px 8px; opacity:.45; min-width:120px; transition:opacity .15s, box-shadow .15s}
.pick button canvas{display:block; width:72px; height:80px; margin:0 auto 4px; image-rendering:pixelated}
.pick button b{display:block; font-weight:400; font-size:19px}
.pick button small{font-size:14px}
.pick button.sel{opacity:1; box-shadow:0 0 18px var(--nv-glow)}
.pick button:focus-visible{outline:2px dashed currentColor; outline-offset:4px}
.fp{all:unset; cursor:pointer; position:relative; width:170px; height:170px; margin:8px auto 4px; border-radius:50%; display:grid; place-items:center;
  background:conic-gradient(var(--nv) calc(var(--p,0)*1%),rgba(255,255,255,.08) 0); touch-action:none; -webkit-user-select:none; user-select:none}
.fp > div{width:150px; height:150px; border-radius:50%; background:var(--nv2); display:grid; place-items:center; position:relative; overflow:hidden}
.fp svg{width:96px; height:96px; color:var(--nv)}
.fp .beam{position:absolute; left:0; right:0; height:3px; background:var(--nv); box-shadow:0 0 10px var(--nv); animation:fpbeam 1.4s ease-in-out infinite alternate}
.fp.idle .beam{animation-play-state:paused; opacity:.3}
.fp:focus-visible{outline:2px dashed var(--nv); outline-offset:6px}
@keyframes fpbeam{from{top:12%}to{top:86%}}
.fpmsg{font-size:19px; letter-spacing:2px; margin:12px 0 0; min-height:52px}
.fpmsg.ok{font-family:"Orbitron","Share Tech Mono",monospace; border:2px solid currentColor; padding:8px; display:inline-block}
@media (max-width:560px){ .file{grid-template-columns:1fr} .lb{grid-template-columns:100px 1fr 56px} .trapstate{font-size:36px} }
</style>
</head>
<body data-agent="spyhunter">
<header>
  <a class="brand" href="#hq">
<pre class="logo" aria-hidden="true"> ____ ___    _
/ ___|_ _|  / \
\___ \| |  / _ \
 ___) | | / ___ \
|____/___/_/   \_\</pre>
    <span class="name"><b>SIA</b>Secret Intelligence Agency</span>
  </a>
  <a class="chip" id="chip" href="#login" title="Switch agent" hidden>
    <canvas id="chip-art" width="72" height="80"></canvas>
    <span><b id="chip-name"></b><small id="chip-rank"></small></span>
  </a>
</header>

<main>
<!-- ===================== LOGIN ===================== -->
<section id="login">
  <div class="nv login">
    <span class="cn c1"></span><span class="cn c2"></span><span class="cn c3"></span><span class="cn c4"></span>
    <div class="rec"><b>●</b> SIA BIOMETRIC GATE</div>
    <div class="nvstep">1 · TAP YOUR AGENT</div>
    <div class="pick" id="agent-cards"></div>
    <div class="nvstep" style="margin-top:22px">2 · HOLD YOUR THUMB ON THE SCANNER</div>
    <button class="fp idle" id="fp" aria-label="Fingerprint scanner: press and hold">
      <div><span class="beam"></span>
        <svg viewBox="0 0 60 60" fill="none" stroke="currentColor" stroke-width="2.4" stroke-linecap="round"><path d="M18 44c-3-4-4-9-4-14 0-9 7-16 16-16s16 7 16 16"/><path d="M24 50c-3-5-4-10-4-20 0-6 4-10 10-10s10 4 10 10c0 4 0 8-1 12"/><path d="M30 30c0 8 1 14 4 20"/><path d="M36 52c2-4 3-9 3-14"/><path d="M12 22c3-7 10-12 18-12 7 0 13 3 16 8"/><path d="M47 34c1 5 0 10-2 14"/></svg>
      </div></button>
    <div class="fpmsg" id="fp-msg" aria-live="polite">SELECT YOUR AGENT, THEN HOLD</div>
  </div>
  <details class="explain" open>
    <summary>🕵️ What is SIA?</summary>
    <p>SIA stands for <b>Secret Intelligence Agency</b>. Agents go on <b>missions</b> using <b>spy gadgets</b>
      (a secret light, a listening bug, a spy camera and a code machine). Every mission you finish earns <b>points</b>,
      and points make you rank up: Recruit → Agent → Special Agent → Master Spy → Legendary Spy!</p>
  </details>
</section>

<!-- ===================== HQ ===================== -->
<section id="hq" hidden>
  <div class="nv">
    <span class="cn c1"></span><span class="cn c2"></span><span class="cn c3"></span><span class="cn c4"></span>
    <div class="rec"><b>●</b> REC · NV MODE</div><div class="clock" id="hq-clock"></div>
    <div class="scope"><i class="h"></i><i class="v"></i><canvas id="hq-art" width="144" height="160"></canvas></div>
    <div class="readout">TARGET LOCKED: <b id="hq-name"></b><br>RANK ....... <b id="hq-rank"></b><br>POINTS ..... <b id="hq-points">0</b></div>
    <div class="nvbar"><i id="hq-progress"></i></div>
    <div class="nvnext" id="hq-next"></div>
    <div class="nvbadges" id="hq-badges"></div>
    <nav class="nvmenu">
      <a href="#missions">[ MISSIONS ]<small>earn points, rank up</small></a>
      <a href="#gadgets">[ GADGETS ]<small>your spy tools</small></a>
      <a href="#trap">[ SOUND TRAP ]<small>catch intruders</small></a>
      <a href="#files">[ CASE FILES ]<small>evidence &amp; log</small></a>
    </nav>
  </div>
  <div class="board">
    <h3>🏆 Top agents</h3>
    <div id="leaderboard"></div>
  </div>
  <p class="status" id="status"></p>
</section>

<!-- ===================== MISSIONS ===================== -->
<section id="missions" hidden>
  <a class="back" href="#hq">← HQ</a>
  <h2>🗂️ Missions</h2>
  <p class="hint">Pick a case file. Each mission uses a spy gadget. Finish it to earn points!</p>
  <div class="files" id="mission-list"></div>
</section>

<!-- ===================== ONE MISSION ===================== -->
<section id="mission" hidden>
  <a class="back" href="#missions">← Missions</a>
  <div class="dossier">
    <span class="stamp">TOP SECRET</span>
    <h3 id="m-title"></h3>
    <p id="m-brief"></p>
    <p style="font-family:var(--round); font-size:17px"><b>Gadget:</b> <span id="m-gadget"></span> &nbsp;·&nbsp; <b>Reward:</b> <span id="m-reward"></span></p>
  </div>
  <div id="m-body"></div>
</section>

<!-- ===================== GADGETS ===================== -->
<section id="gadgets" hidden>
  <a class="back" href="#hq">← HQ</a>
  <h2>🧰 Spy gadgets</h2>
  <p class="hint">Practice with your tools here. Missions use them too!</p>
  <div class="tools">
    <a href="#g-lamp"><div class="screen"><span class="ico">🔦</span><br>SIGNAL LAMP<small>Morse code on the light</small></div></a>
    <a href="#g-bug"><div class="screen"><span class="ico">👂</span><br>LISTENING BUG<small>How loud is it?</small></div></a>
    <a href="#g-cam"><div class="screen"><span class="ico">📷</span><br>SPY CAM<small>See and snap evidence</small></div></a>
    <a href="#g-code"><div class="screen"><span class="ico">🔐</span><br>CODE MACHINE<small>Scramble secret messages</small></div></a>
    <a href="#trap"><div class="screen"><span class="ico">🚨</span><br>SOUND TRAP<small>Noise starts the camera</small></div></a>
    <a href="#files"><div class="screen"><span class="ico">📁</span><br>CASE FILES<small>Photos on the memory card</small></div></a>
  </div>
</section>

<section id="g-lamp" hidden>
  <a class="back" href="#gadgets">← Gadgets</a>
  <h2>🔦 Signal lamp</h2>
  <p class="hint">Type a word. petbot flashes it in Morse code with its little orange light (next to the USB plug).</p>
  <div class="row"><input type="text" id="lamp-text" maxlength="20" placeholder="HELLO"><button id="lamp-go">🔦 Flash it</button><button class="ghost" id="lamp-stop">Stop</button></div>
  <div class="screen" id="lamp-code">·· type something ··</div>
  <p class="hint" style="margin-top:14px">Short flash = dot •&nbsp;&nbsp; Long flash = dash —</p>
  <div class="morse" id="lamp-chart"></div>
</section>

<section id="g-bug" hidden>
  <a class="back" href="#gadgets">← Gadgets</a>
  <h2>👂 Listening bug</h2>
  <p class="hint">petbot's microphone. Whisper, talk, clap: how loud can you go?</p>
  <div class="screen"><div>LOUDNESS <span id="bug-level">0</span></div><div class="meter"><i id="bug-meter"></i></div>
    <div style="margin-top:8px">LOUDEST <span id="bug-best">0</span> &nbsp; <span id="bug-word" class="cursor"></span></div></div>
</section>

<section id="g-cam" hidden>
  <a class="back" href="#gadgets">← Gadgets</a>
  <h2>📷 Spy cam</h2>
  <p class="hint">Live from petbot's camera. Snapped photos go into the case files on the memory card.</p>
  <div class="cam"><img id="cam-img" alt="Spy cam"><span class="rec">REC</span></div>
  <div class="row"><button id="cam-snap">📸 Snap evidence</button><button class="ghost" id="cam-flip">🙃 Flip</button></div>
  <p class="hint" id="cam-msg"></p>
  <div class="photos" id="cam-photos"></div>
</section>

<section id="g-code" hidden>
  <a class="back" href="#gadgets">← Gadgets</a>
  <h2>🔐 Code machine</h2>
  <p class="hint">A secret code where every letter moves forward in the alphabet. Pick how far with the slider.
    Top row = real letters, bottom row = secret letters.</p>
  <div class="row"><label style="font-size:20px">Shift: <b id="code-shift-v">3</b></label><input type="range" id="code-shift" min="1" max="25" value="3"></div>
  <div class="screen wheel" id="code-wheel"></div>
  <div class="row">
    <input type="text" id="code-in" maxlength="12" placeholder="MEET AT TEN">
    <button class="ghost" id="code-mode">Mode: scramble 🔒</button>
  </div>
  <div class="screen"><span style="color:var(--muted)">RESULT:</span> <span id="code-out" class="big"></span></div>
  <div class="row"><button id="code-send">📟 Send to petbot's screen</button></div>
  <p class="hint" id="code-msg"></p>
</section>

<!-- ===================== SOUND TRAP ===================== -->
<section id="trap" hidden>
  <a class="back" href="#hq">← HQ</a>
  <h2>🚨 Sound trap</h2>
  <p class="hint">Arm the trap and hide! If anyone makes a noise, petbot's spy cam wakes up and snaps 3 evidence photos.
    Catching an intruder earns the <b>Trap Master</b> points.</p>
  <div class="screen">
    <div class="trapstate" id="trap-state">DISARMED</div>
    <div style="font-size:22px; color:var(--muted)" id="trap-sub">Ready when you are, agent.</div>
    <div class="meter" style="margin-top:14px"><i id="trap-meter"></i><span class="thr" id="trap-thr"></span></div>
    <div style="font-size:20px; margin-top:4px">Red line = how loud a noise sets off the trap</div>
  </div>
  <div class="row">
    <label style="font-size:20px">Sensitivity: <b id="trap-sens-v"></b></label>
    <input type="range" id="trap-sens" min="1" max="10" value="5">
  </div>
  <div class="row"><button id="trap-arm">🚨 Arm the trap</button><button class="ghost" id="trap-disarm">Disarm</button></div>
  <div id="trap-live" hidden>
    <h3>📷 Spy cam woke up!</h3>
    <div class="cam"><img id="trap-cam" alt="Spy cam"><span class="rec">LIVE</span></div>
  </div>
  <h3 style="margin-top:22px">🗂️ Caught on camera</h3>
  <div id="trap-events"><p class="hint">No intruders yet.</p></div>
</section>

<!-- ===================== CASE FILES ===================== -->
<section id="files" hidden>
  <a class="back" href="#hq">← HQ</a>
  <h2>📁 Case files</h2>
  <p class="hint">Evidence photos and the mission log, stored on petbot's memory card.</p>
  <div class="photos" id="files-photos"></div>
  <div class="dossier"><span class="stamp">CLASSIFIED</span><h3>Mission log</h3><pre class="log" id="files-log">Loading…</pre></div>
</section>
</main>

<div class="overlay" id="overlay" hidden></div>

<script>
// ART-BEGIN
const UNICORN_SPY = [['000000000','000000000','000000010','000000030','000040060','0000400c0','000460040','001ff0380','003ff0700','007fd8f00','007fcdc00','00ffcf800','01ffffe00','010700000','000000000','03800e000','03d80e000','07e006008','07010000c','070c0000e','077cffffe','0ef8ffffe','0ef87ffe7','0df87ffff','0df9ffffe','1df1ffffe','1df1ffcee','1df1fef1c','1df1fe7f8','3de1fe0c0','3de1ff000','3de1ff000','3fe1ff000','7bc1ff000','7fc1ff000','77c3ff000','7fc3ff800','6f83ff800','6f83ff800','7fc3ff800'],
  ['000000000','000000000','000000010','000000030','000040060','0000400c0','000460040','001ff0380','003ff0700','007fd8f00','007fcdc00','00ffcf800','010fffe00','010300000','010000000','03c00e000','03d80e000','07f006008','07800000c','07040000e','073cffffe','0ef8ffffe','0ef87ffe7','0df87ffff','0df9ffffe','1df1ffffe','1df1ffcee','1df1fef1c','1df1fe7f8','3de1fe0c0','3de1ff000','3de1ff000','3fe1ff000','7bc1ff000','7fc1ff000','77c3ff000','7fc3ff800','6f83ff800','6f83ff800','7fc3ff800'],
  ['000000000','000000000','000000010','000000030','000040060','0000400c0','000460040','001ff0380','003ff0700','007fd8f00','007fcdc00','00bfcf800','0107ffe00','000100000','01c000000','03d00e000','03dc0e000','07f006008','07a00000c','07840000e','070cffffe','0e38ffffe','0ef87ffe7','0df87ffff','0df9ffffe','1df1ffffe','1df1ffcee','1df1fef1c','1df1fe7f8','3de1fe0c0','3de1ff000','3de1ff000','3fe1ff000','7bc1ff000','7fc1ff000','77c3ff000','7fc3ff800','6f83ff800','6f83ff800','7fc3ff800'],
  ['000000000','000000000','000000010','000000030','000040060','0000400c0','000460040','001ff0380','003ff0700','007fd8f00','007fcdc00','00ffcf800','010fffe00','010300000','010000000','03c00e000','03d80e000','07f006008','07800000c','07040000e','073cffffe','0ef8ffffe','0ef87ffe7','0df87ffff','0df9ffffe','1df1ffffe','1df1ffcee','1df1fef1c','1df1fe7f8','3de1fe0c0','3de1ff000','3de1ff000','3fe1ff000','7bc1ff000','7fc1ff000','77c3ff000','7fc3ff800','6f83ff800','6f83ff800','7fc3ff800']];
const AGENT_SPY = [['003fffc00','003fffc00','003fffc00','003fffc00','003fffc00','003fffc00','003fffc00','000000000','000000000','001fff800','03fffffc0','03fffffc0','01fffff80','000fff000','000fff000','001861800','001861800','001861800','001cf3800','001fff800','001fff800','001fff800','000fff000','000f07000','0007fe000','0003fc000','0007fe000','0039f9c00','00fcf1f00','01fc23f80','03fe23fc0','03ff0ffc0','03ff9ffc0','07ffdffe0','07fffffe0','07fffffe0','0fffffff0','0fffffff0','0fffffff8','1fffffff8'],
  ['003fffc00','003fffc00','003fffc00','003fffc00','003fffc00','003fffc00','003fffc00','000000000','000000000','001fff800','03fffffc0','03fffffc0','01fffff80','000fff000','000fff000','001861800','001c71800','001861800','001cf3800','001fff800','001fff800','001fff800','000fff000','000f07000','0007fe000','0003fc000','0007fe000','0039f9c00','00fcf1f00','01fc23f80','03fe23fc0','03ff0ffc0','03ff9ffc0','07ffdffe0','07fffffe0','07fffffe0','0fffffff0','0fffffff0','0fffffff8','1fffffff8'],
  ['003fffc00','003fffc00','003fffc00','003fffc00','003fffc00','003fffc00','003fffc00','000000000','000000000','001fff800','03fffffc0','03fffffc0','01fffff80','000fff000','000fff000','001861800','001a69800','001861800','001cf3800','001fff800','001fff800','001fff800','000fff000','000f07000','0007fe000','0003fc000','0007fe000','0039f9c00','00fcf1f00','01fc23f80','03fe23fc0','03ff0ffc0','03ff9ffc0','07ffdffe0','07fffffe0','07fffffe0','0fffffff0','0fffffff0','0fffffff8','1fffffff8'],
  ['003fffc00','003fffc00','003fffc00','003fffc00','003fffc00','003fffc00','003fffc00','000000000','000000000','001fff800','03fffffc0','03fffffc0','01fffff80','000fff000','000fff000','001861800','001a69800','001861800','001cf3800','001fff800','001fff800','001fff800','000fff000','000f07000','0007fe000','0003fc000','0007fe000','0039f9c00','00fcf1f00','01fc23f80','03fe23fc0','03ff0ffc0','03ff9ffc0','07ffdffe0','07fffffe0','07fffffe0','0fffffff0','0fffffff0','0fffffff8','1fffffff8']];
// ART-END

const $ = id => document.getElementById(id);
const api = path => fetch(path, {cache: 'no-store'}).then(r => r.json());
const esc = s => String(s).replace(/[&<>"]/g, c => ({'&': '&amp;', '<': '&lt;', '>': '&gt;', '"': '&quot;'}[c]));

// ---------- agents, ranks, missions ----------
const AGENTS = {
  megaspy:   {icon: '🦄', color: '#FF5FA2', hi: '#FFC2DD', art: () => UNICORN_SPY, theme: 'unicorn theme'},
  spyhunter: {icon: '🕵️', color: '#39FF9E', hi: '#B8FFD9', art: () => AGENT_SPY, theme: 'classic spy'},
};
const RANKS = [[0, 'Recruit', '🥚'], [100, 'Agent', '🕶️'], [250, 'Special Agent', '⭐'], [500, 'Master Spy', '👑'], [1000, 'Legendary Spy', '🏆']];
function rankOf(points, id) {
  let i = 0;
  while (i + 1 < RANKS.length && points >= RANKS[i + 1][0]) i++;
  const [, name, icon] = RANKS[i];
  const special = i === RANKS.length - 1 && id === 'megaspy';
  return {name: special ? 'Legendary Unicorn Spy' : name, icon: special ? '🦄' : icon, next: RANKS[i + 1], floor: RANKS[i][0]};
}
const MISSIONS = [
  {id: 'signal', name: 'Signal Rookie', icon: '🔦', pts: 50, gadget: 'Signal lamp',
   brief: "Enemy spies are sending a secret word with a flashing light! Watch petbot's little orange light. Short flash = dot, long flash = dash. Use the Morse code chart to work out the word."},
  {id: 'ghost', name: 'Ghost Walk', icon: '👻', pts: 30, gadget: 'Listening bug',
   brief: "A real spy moves without a sound. Stay super quiet near petbot for 20 seconds. If petbot's listening bug hears you, the clock starts over!"},
  {id: 'code', name: 'Code Breaker', icon: '🔐', pts: 40, gadget: 'Code machine',
   brief: "We caught a scrambled message on petbot's screen! Every letter was moved forward in the alphabet. Use the code machine to move the letters back and read it."},
  {id: 'courier', name: 'Secret Courier', icon: '✉️', pts: 10, gadget: 'Code machine',
   brief: "Send a secret message to the other agent. It appears on petbot's screen, scrambled, so only an agent with the code machine can read it!"},
  {id: 'trap', name: 'Trap Master', icon: '🚨', pts: 40, gadget: 'Sound trap',
   brief: "Guard your room with the sound trap. If an intruder makes a noise, petbot's spy cam snaps 3 evidence photos. Catch someone to earn the points!"},
  {id: 'evidence', name: 'Evidence Hunt', icon: '📸', pts: 20, gadget: 'Spy cam',
   brief: "HQ needs a photo of a secret object. Find it, hold it up to petbot's spy cam, and snap the evidence!"},
];
const MORSE = {A:'.-',B:'-...',C:'-.-.',D:'-..',E:'.',F:'..-.',G:'--.',H:'....',I:'..',J:'.---',K:'-.-',L:'.-..',M:'--',
  N:'-.',O:'---',P:'.--.',Q:'--.-',R:'.-.',S:'...',T:'-',U:'..-',V:'...-',W:'.--',X:'-..-',Y:'-.--',Z:'--..'};
const pretty = code => code.replace(/\./g, '•').replace(/-/g, '—');

let agent = null, state = null;
try { agent = localStorage.getItem('sia-agent'); } catch (e) {}
const linkAgent = new URLSearchParams(location.search).get('agent');  // bookmark: http://sia.local/?agent=megaspy
if (linkAgent) agent = linkAgent;
if (agent && !AGENTS[agent]) agent = null;
const me = () => state && state.agents.find(a => a.id === agent);

// ---------- pixel-art mascots (the same bitmaps petbot's OLED shows) ----------
const toBits = rows => rows.map(h => parseInt(h, 16).toString(2).padStart(36, '0'));
const FRAMES = {};
function frames(id) { return FRAMES[id] || (FRAMES[id] = AGENTS[id].art().map(toBits)); }
function drawMascot(cv, id, t) {
  const g = cv.getContext('2d'), px = cv.width / 36, a = AGENTS[id];
  const phase = Math.floor(t / 150) % 20;
  const rows = frames(id)[id === 'megaspy' ? Math.floor(t / 200) % 4 : phase < 4 ? phase : 0];
  g.clearRect(0, 0, cv.width, cv.height);
  g.fillStyle = a.color;
  rows.forEach((row, y) => { for (let x = 0; x < 36; x++) if (row[x] === '1') {
    if (px < 3) { g.fillRect(x * px, y * px, px, px); continue; }  // tiny canvases: solid pixels
    g.beginPath(); g.roundRect(x * px + 0.4, y * px + 0.4, px - 0.8, px - 0.8, px / 4); g.fill(); } });
  if (id === 'megaspy') {  // unicorn theme mode: a little sparkle by the horn
    const k = Math.floor(t / 300) % 4;
    g.fillStyle = k % 2 ? '#FFC940' : a.hi;
    const r = (k === 0 ? 1.6 : 0.9) * px, cx = 26 * px, cy = 3 * px;
    g.fillRect(cx - r, cy - px / 5, 2 * r, px / 2.5); g.fillRect(cx - px / 5, cy - r, px / 2.5, 2 * r);
  }
}

// ---------- router ----------
const SECTIONS = ['login', 'hq', 'missions', 'mission', 'gadgets', 'g-lamp', 'g-bug', 'g-cam', 'g-code', 'trap', 'files'];
let timers = [], cleanups = [];
function every(ms, fn) { fn(); timers.push(setInterval(fn, ms)); }
function onLeave(fn) { cleanups.push(fn); }
function route() {
  let h = location.hash.slice(1) || 'hq', param = null;
  if (h.startsWith('m-')) { param = h.slice(2); h = 'mission'; }
  if (!agent && h !== 'login') { location.hash = '#login'; return; }
  if (!SECTIONS.includes(h)) h = 'hq';
  timers.forEach(clearInterval); timers = [];
  cleanups.forEach(f => f()); cleanups = [];
  SECTIONS.forEach(s => $(s).hidden = s !== h);
  window.scrollTo(0, 0);
  if (agent) refresh().catch(() => {});  // keeps the agent chip in the header current
  START[h](param);
  every(120, () => document.querySelectorAll('canvas[data-mascot]').forEach(c => drawMascot(c, c.dataset.mascot, performance.now())));
}
addEventListener('hashchange', route);

// ---------- state & shared UI ----------
function refresh() {
  return api('/api/state').then(s => {
    const before = me() ? me().points : null;
    state = s;
    renderChip();
    return before;
  });
}
function renderChip() {
  const m = me();
  $('chip').hidden = !m;
  if (!m) return;
  $('chip-name').textContent = m.id;
  const r = rankOf(m.points, m.id);
  $('chip-rank').textContent = `${r.icon} ${r.name} · ${m.points} pts`;
  $('chip-art').dataset.mascot = m.id;
}
function setAgent(id) {
  agent = id;
  try { localStorage.setItem('sia-agent', id); } catch (e) {}
  document.body.dataset.agent = id;
  fetch('/api/agent?id=' + id).catch(() => {});
}

function celebrate(points, text) {
  const o = $('overlay');
  o.innerHTML = `<div class="reward"><b>+${points}</b><span>${esc(text || 'points!')}</span></div>`;
  o.hidden = false;
  const bits = agent === 'megaspy' ? ['🦄', '💖', '✨', '🌈', '⭐'] : ['🕶️', '⭐', '🎉', '💥', '🏆'];
  for (let i = 0; i < 28; i++) {
    const c = document.createElement('div');
    c.className = 'confetti';
    c.textContent = bits[i % bits.length];
    c.style.left = Math.random() * 100 + 'vw';
    c.style.animationDuration = 1.8 + Math.random() * 1.8 + 's';
    document.body.appendChild(c);
    setTimeout(() => c.remove(), 4000);
  }
  setTimeout(() => o.hidden = true, 2600);
  refresh();
}

let audio;
function siren() {
  try {
    audio = audio || new (window.AudioContext || window.webkitAudioContext)();
    const o = audio.createOscillator(), g = audio.createGain();
    o.type = 'square'; g.gain.value = 0.05;
    o.connect(g); g.connect(audio.destination);
    const t = audio.currentTime;
    for (let i = 0; i < 6; i++) { o.frequency.setValueAtTime(880, t + i * 0.3); o.frequency.setValueAtTime(620, t + i * 0.3 + 0.15); }
    o.start(t); o.stop(t + 1.8);
  } catch (e) {}
}

const loud01 = l => Math.min(1, Math.max(0, (Math.log10(Math.max(l, 10)) - 1) / 2.6));
const camUrl = () => 'http://' + location.hostname + ':81/stream';

// ---------- LOGIN: pick your agent, then hold your thumb on the fingerprint scanner ----------
const HOLD_MS = 1500;
function startLogin() {
  let chosen = agent || 'megaspy', holding = false, t0 = 0, raf = 0, done = false;
  const fp = $('fp'), msg = $('fp-msg');
  const choose = id => {
    chosen = id;
    document.body.dataset.agent = id;  // preview that agent's colours
    document.querySelectorAll('.pick button').forEach(b => b.classList.toggle('sel', b.dataset.id === id));
    msg.className = 'fpmsg'; msg.textContent = `AGENT ${id.toUpperCase()} · HOLD YOUR THUMB ON THE SCANNER`;
  };
  refresh().catch(() => {}).then(() => {
    const list = state ? state.agents : Object.keys(AGENTS).map(id => ({id, points: 0}));
    $('agent-cards').innerHTML = list.map(a => {
      const r = rankOf(a.points, a.id);
      return `<button data-id="${a.id}"><canvas width="72" height="80" data-mascot="${a.id}"></canvas><b>${a.id}</b><small>${r.icon} ${r.name} · ${a.points} pts</small></button>`;
    }).join('');
    document.querySelectorAll('.pick button').forEach(b => b.onclick = () => choose(b.dataset.id));
    choose(chosen);
  });
  const tick = () => {
    if (!holding) return;
    const p = Math.min(1, (performance.now() - t0) / HOLD_MS);
    fp.style.setProperty('--p', p * 100);
    if (p >= 1) {
      holding = false; done = true;
      msg.className = 'fpmsg ok'; msg.textContent = `ACCESS GRANTED · WELCOME, AGENT ${chosen.toUpperCase()}`;
      setAgent(chosen);
      setTimeout(() => { location.hash = '#hq'; }, 900);
      return;
    }
    raf = requestAnimationFrame(tick);
  };
  const press = e => {
    if (done || holding) return;
    if (e) e.preventDefault();
    holding = true; t0 = performance.now(); fp.classList.remove('idle');
    msg.className = 'fpmsg'; msg.textContent = 'SCANNING... KEEP HOLDING';
    raf = requestAnimationFrame(tick);
  };
  const release = () => {
    if (!holding) return;
    holding = false; cancelAnimationFrame(raf);
    fp.style.setProperty('--p', 0); fp.classList.add('idle');
    msg.className = 'fpmsg'; msg.textContent = 'TOO QUICK! HOLD YOUR THUMB STILL';
  };
  fp.onpointerdown = press; fp.onpointerup = release; fp.onpointerleave = release; fp.onpointercancel = release;
  fp.oncontextmenu = e => e.preventDefault();
  fp.onkeydown = e => { if ((e.key === ' ' || e.key === 'Enter') && !e.repeat) press(e); };
  fp.onkeyup = e => { if (e.key === ' ' || e.key === 'Enter') release(); };
  onLeave(() => { holding = false; cancelAnimationFrame(raf); fp.style.setProperty('--p', 0); fp.classList.add('idle'); if (agent) document.body.dataset.agent = agent; });
}

// ---------- HQ: the night-vision screen ----------
function startHq() {
  $('hq-art').dataset.mascot = agent;
  every(1000, () => { const d = new Date(); $('hq-clock').textContent = [d.getHours(), d.getMinutes(), d.getSeconds()].map(n => String(n).padStart(2, '0')).join(':'); });
  every(3000, () => refresh().then(() => {
    const m = me(), r = rankOf(m.points, m.id);
    $('hq-name').textContent = m.id.toUpperCase();
    $('hq-rank').textContent = `${r.name.toUpperCase()} ${r.icon}`;
    $('hq-points').textContent = m.points;
    if (r.next) {
      $('hq-progress').style.width = (m.points - r.floor) / (r.next[0] - r.floor) * 100 + '%';
      $('hq-next').textContent = `${r.next[0] - m.points} MORE POINTS → ${r.next[1].toUpperCase()} ${r.next[2]}`;
    } else {
      $('hq-progress').style.width = '100%';
      $('hq-next').textContent = 'TOP RANK REACHED. LEGENDARY!';
    }
    $('hq-badges').innerHTML = MISSIONS.map((ms, i) =>
      `<span class="${m.done[i] ? '' : 'locked'}" title="${ms.name}">${ms.icon} ${m.done[i] ? '×' + m.done[i] : '--'}</span>`).join('');
    const top = Math.max(1, ...state.agents.map(a => a.points));
    $('leaderboard').innerHTML = state.agents.slice().sort((a, b) => b.points - a.points).map(a =>
      `<div class="lb"><span>${AGENTS[a.id].icon} ${a.id}</span><span class="bar"><i style="width:${a.points / top * 100}%; background:${AGENTS[a.id].color}"></i></span><span class="v">${a.points}</span></div>`).join('');
    const ok = v => v ? '✅' : '❌';
    $('status').textContent = `📷 spy cam ${ok(state.camera)} · 👂 bug ${ok(state.mic)} · 📟 screen ${ok(state.screen)} · 🕒 ${state.time}`;
  }));
}

// ---------- MISSIONS ----------
function startMissions() {
  refresh().then(() => {
    const m = me();
    $('mission-list').innerHTML = MISSIONS.map((ms, i) => `<a href="#m-${ms.id}"><div class="dossier">
      <span class="stamp">TOP SECRET</span><div class="ico">${ms.icon}</div><h3>${ms.name}</h3>
      <div class="meta">+${ms.pts} points · ${ms.gadget}${m.done[i] ? ' · ✅ done ×' + m.done[i] : ''}</div></div></a>`).join('');
  });
}

function startMission(id) {
  const ms = MISSIONS.find(x => x.id === id);
  if (!ms) { location.hash = '#missions'; return; }
  $('m-title').textContent = `${ms.icon} MISSION: ${ms.name.toUpperCase()}`;
  $('m-brief').textContent = ms.brief;
  $('m-gadget').textContent = ms.gadget;
  $('m-reward').textContent = `+${ms.pts} points`;
  MISSION_UI[id]();
  onLeave(() => {
    if (['signal', 'ghost', 'code', 'evidence'].includes(id)) fetch('/api/mission/stop').catch(() => {});
    $('m-body').innerHTML = '';
  });
}

function startButton(label, fn) {
  $('m-body').innerHTML = `<div class="row"><button id="m-start">▶ ${label}</button></div>`;
  $('m-start').onclick = fn;
}
function again(label, fn) {
  $('m-body').insertAdjacentHTML('beforeend', `<div class="row"><button id="again">🔁 ${label}</button></div>`);
  $('again').onclick = fn;
}
const startOn = id => api(`/api/mission/start?id=${id}&agent=${agent}`);

const MISSION_UI = {
  signal() {
    startButton('Start mission', () => startOn('signal').then(r => {
      $('m-body').innerHTML = `
        <div class="screen"><div class="cursor">INCOMING SIGNAL... WATCH PETBOT'S LIGHT</div>
          <div class="slots">${'<span></span>'.repeat(r.letters)}</div>
          <div style="font-size:22px;color:var(--term-dim)">The secret word has ${r.letters} letters. The signal repeats, take your time!</div>
          <div id="sig-hint" style="font-size:32px"></div></div>
        <div class="row"><input type="text" id="sig-answer" maxlength="8" placeholder="WORD?"><button id="sig-check">✔ Check</button>
          <button class="ghost" id="sig-hint-btn">💡 Hint (half points)</button></div>
        <p class="hint" id="sig-msg"></p>
        <div class="morse">${Object.entries(MORSE).map(([k, v]) => `<div><b>${k}</b>${pretty(v)}</div>`).join('')}</div>`;
      $('sig-hint-btn').onclick = () => api('/api/mission/hint').then(h => {
        $('sig-hint').textContent = 'INTERCEPTED: ' + pretty(h.morse);
        $('sig-hint-btn').disabled = true;
      });
      const check = () => api('/api/mission/answer?answer=' + encodeURIComponent($('sig-answer').value)).then(a => {
        if (a.correct) { $('sig-msg').textContent = '✅ Signal decoded! Great work, agent.'; celebrate(a.points, 'Signal decoded!');
          again('New signal', MISSION_UI.signal); }
        else $('sig-msg').textContent = '❌ Not quite. Watch the light again and check the chart!';
      });
      $('sig-check').onclick = check;
      $('sig-answer').onkeydown = e => { if (e.key === 'Enter') check(); };
    }));
  },

  ghost() {
    startButton('Start the 20 seconds', () => startOn('ghost').then(() => {
      $('m-body').innerHTML = `
        <div class="ring" id="gh-ring"><div id="gh-sec">20</div></div>
        <div class="screen"><div id="gh-msg" class="cursor">SHHH... STAY SUPER QUIET</div><div class="meter"><i id="gh-meter"></i></div></div>`;
      let lastQuiet = 0, finished = false;
      every(250, () => {
        if (finished) return;
        api('/api/mission/state').then(s => {
          if (!s || finished) return;
          const left = Math.max(0, Math.ceil((s.goalMs - s.quietMs) / 1000));
          $('gh-sec').textContent = left;
          $('gh-ring').style.setProperty('--p', s.quietMs / s.goalMs * 100);
          if (s.quietMs + 400 < lastQuiet) $('gh-msg').textContent = '👂 PETBOT HEARD YOU! STARTING OVER...';
          else if (s.quietMs > 1500) $('gh-msg').textContent = 'SHHH... STAY SUPER QUIET';
          lastQuiet = s.quietMs;
          if (s.done) {
            finished = true;
            $('gh-msg').textContent = '👻 YOU ARE A GHOST. MISSION COMPLETE!';
            celebrate(30, 'Silent as a ghost!');
            again('Try again', MISSION_UI.ghost);
          }
        });
        api('/api/sound').then(s => $('gh-meter').style.width = loud01(s.peak) * 100 + '%');
      });
    }));
  },

  code() {
    startButton('Show the message', () => startOn('code').then(r => {
      $('m-body').innerHTML = `
        <div class="screen"><div style="color:var(--term-dim)">INTERCEPTED MESSAGE (also on petbot's screen):</div>
          <div class="big">${esc(r.cipher)}</div>
          <div>Every letter moved <b>${r.shift}</b> forward. Move them back!</div></div>
        <p class="hint" style="margin-top:14px">Find each secret letter in the <b>bottom</b> row. The real letter is right above it.</p>
        <div class="screen wheel" id="cb-wheel"></div>
        <div class="row"><input type="text" id="cb-answer" maxlength="14" placeholder="REAL MESSAGE"><button id="cb-check">✔ Check</button></div>
        <p class="hint" id="cb-msg"></p>`;
      $('cb-wheel').innerHTML = wheelHtml(r.shift);
      const check = () => api('/api/mission/answer?answer=' + encodeURIComponent($('cb-answer').value)).then(a => {
        if (a.correct) { $('cb-msg').textContent = '✅ Code cracked!'; celebrate(a.points, 'Code cracked!');
          again('New message', MISSION_UI.code); }
        else $('cb-msg').textContent = '❌ Not yet. Check each letter against the code machine!';
      });
      $('cb-check').onclick = check;
      $('cb-answer').onkeydown = e => { if (e.key === 'Enter') check(); };
    }));
  },

  courier() {
    const other = Object.keys(AGENTS).find(k => k !== agent);
    $('m-body').innerHTML = `
      <p class="hint" style="margin-top:18px">Write a message for <b>${other}</b> (up to 12 letters), pick a secret shift, and send it to petbot's screen.</p>
      <div class="row"><input type="text" id="cr-text" maxlength="12" placeholder="MEET AT TEN"></div>
      <div class="row"><label style="font-size:20px">Secret shift: <b id="cr-shift-v">3</b></label><input type="range" id="cr-shift" min="1" max="25" value="3"></div>
      <div class="screen">SCRAMBLED: <span class="big" id="cr-out"></span></div>
      <div class="row"><button id="cr-send">📟 Send it</button></div>
      <p class="hint" id="cr-msg">Tip: tell ${other} the shift number in secret, so only they can decode it!</p>`;
    const upd = () => { $('cr-shift-v').textContent = $('cr-shift').value; $('cr-out').textContent = caesar($('cr-text').value.toUpperCase(), +$('cr-shift').value); };
    $('cr-text').oninput = upd; $('cr-shift').oninput = upd; upd();
    $('cr-send').onclick = () => sendCourier($('cr-text').value, +$('cr-shift').value, 'cr-msg');
  },

  trap() {
    $('m-body').innerHTML = `<div class="row" style="margin-top:20px"><a class="btn" href="#trap">🚨 Go to the sound trap</a></div>
      <p class="hint">Arm it, hide, and wait for an intruder to make a noise. The first catch earns the points!</p>`;
  },

  evidence() {
    startButton('Get my target', () => startOn('evidence').then(r => {
      $('m-body').innerHTML = `
        <div class="screen"><div style="color:var(--term-dim)">YOUR TARGET:</div><div class="target">${esc(r.target)}</div></div>
        <p class="hint" style="margin-top:14px">Find it, hold it up to petbot's camera, then snap!</p>
        <div class="cam"><img id="ev-cam" alt="Spy cam"><span class="rec">REC</span></div>
        <div class="row"><button id="ev-snap">📸 Snap evidence</button></div><p class="hint" id="ev-msg"></p>`;
      $('ev-cam').src = camUrl();
      onLeave(() => $('ev-cam') && $('ev-cam').removeAttribute('src'));
      $('ev-snap').onclick = () => {
        $('ev-snap').disabled = true;
        api('/api/snap?agent=' + agent).then(s => {
          if (s.error) { $('ev-msg').textContent = '❌ ' + s.error; $('ev-snap').disabled = false; return; }
          $('ev-msg').textContent = `✅ Evidence saved as case #${s.case}.`;
          if (s.points) celebrate(s.points, 'Evidence collected!');
          again('New target', MISSION_UI.evidence);
        });
      };
    }));
  },
};

// ---------- code machine helpers ----------
const ABC = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ';
const caesar = (s, k) => s.replace(/[A-Z]/g, c => ABC[(ABC.indexOf(c) + k + 26) % 26]);
function wheelHtml(k) {
  return `<div class="top">${[...ABC].map(c => `<span>${c}</span>`).join('')}</div>
          <div class="bot">${[...ABC].map(c => `<span>${caesar(c, k)}</span>`).join('')}</div>`;
}
function sendCourier(text, shift, msgId) {
  if (!text.trim()) { $(msgId).textContent = 'Write a message first!'; return; }
  api(`/api/courier?agent=${agent}&shift=${shift}&text=${encodeURIComponent(text)}`).then(r => {
    if (r.error) { $(msgId).textContent = '❌ ' + r.error; return; }
    $(msgId).textContent = `📟 Sent! petbot's screen shows "${r.cipher}".` + (r.points ? '' : ` (Courier points again in ${r.wait}s.)`);
    if (r.points) celebrate(r.points, 'Message delivered!');
  });
}

// ---------- GADGETS ----------
function startGadgets() {}
function startLamp() {
  $('lamp-chart').innerHTML = Object.entries(MORSE).map(([k, v]) => `<div><b>${k}</b>${pretty(v)}</div>`).join('');
  const show = () => {
    const t = $('lamp-text').value.toUpperCase();
    $('lamp-code').textContent = t ? [...t].map(c => MORSE[c] ? pretty(MORSE[c]) : c === ' ' ? '/' : '').join('   ') : '·· type something ··';
  };
  $('lamp-text').oninput = show; show();
  $('lamp-go').onclick = () => $('lamp-text').value.trim() && fetch('/api/lamp?text=' + encodeURIComponent($('lamp-text').value));
  $('lamp-stop').onclick = () => fetch('/api/lamp');
}
function startBug() {
  let best = 0, shown = 0;
  every(100, () => api('/api/sound').then(s => {
    shown = Math.max(s.peak, shown * 0.8);
    best = Math.max(best, s.peak);
    $('bug-level').textContent = Math.round(shown);
    $('bug-best').textContent = best;
    $('bug-meter').style.width = loud01(shown) * 100 + '%';
    $('bug-word').textContent = shown < 40 ? 'silence...' : shown < 250 ? 'voices detected' : shown < 1200 ? 'LOUD!' : 'SUPER LOUD!!';
  }).catch(() => {}));
}
function startCam() {
  $('cam-img').src = camUrl();
  onLeave(() => $('cam-img').removeAttribute('src'));
  $('cam-flip').onclick = () => $('cam-img').classList.toggle('flip');
  $('cam-snap').onclick = () => api('/api/snap?agent=' + agent).then(s => {
    if (s.error) { $('cam-msg').textContent = '❌ ' + s.error; return; }
    $('cam-msg').textContent = `✅ Saved as case #${s.case}.`;
    $('cam-photos').insertAdjacentHTML('afterbegin', `<a href="/sd?f=${s.file}" target="_blank"><img src="/sd?f=${s.file}" alt="Evidence"><span>#${s.case}</span></a>`);
  });
}
let codeDecode = false;
function startCode() {
  const upd = () => {
    const k = +$('code-shift').value;
    $('code-shift-v').textContent = k;
    $('code-wheel').innerHTML = wheelHtml(k);
    const t = $('code-in').value.toUpperCase();
    $('code-out').textContent = caesar(t, codeDecode ? -k : k);
    $('code-mode').textContent = codeDecode ? 'Mode: unscramble 🔓' : 'Mode: scramble 🔒';
    $('code-send').hidden = codeDecode;
  };
  $('code-shift').oninput = upd; $('code-in').oninput = upd;
  $('code-mode').onclick = () => { codeDecode = !codeDecode; upd(); };
  $('code-send').onclick = () => sendCourier($('code-in').value, +$('code-shift').value, 'code-msg');
  upd();
}

// ---------- SOUND TRAP ----------
const SENS_WORDS = ['', 'Only big crashes', 'Loud bangs', 'Slammed doors', 'Loud voices', 'Talking', 'Footsteps',
  'Soft steps', 'Quiet voices', 'Whispers', 'A mouse sneezing!'];
function startTrap() {
  let lastCatches = null, liveTimer = null, first = true;
  $('trap-sens').oninput = () => { $('trap-sens-v').textContent = SENS_WORDS[$('trap-sens').value]; fetch('/api/trap?sens=' + $('trap-sens').value); };
  $('trap-arm').onclick = () => {
    try { audio = audio || new (window.AudioContext || window.webkitAudioContext)(); audio.resume(); } catch (e) {}  // allow the siren later
    api(`/api/trap?arm=1&agent=${agent}&sens=${$('trap-sens').value}`);
  };
  $('trap-disarm').onclick = () => api('/api/trap?arm=0');
  onLeave(() => { $('trap-cam').removeAttribute('src'); $('trap-live').hidden = true; clearTimeout(liveTimer);
    document.querySelectorAll('.alarm,.alarm-banner').forEach(e => e.remove()); });
  const photoUrl = (c, i) => `/sd?f=case_${String(c).padStart(4, '0')}_${i}.jpg`;
  every(400, () => api('/api/trap').then(t => {
    if (first) { $('trap-sens').value = t.sens; $('trap-sens-v').textContent = SENS_WORDS[t.sens]; first = false; }
    $('trap-meter').style.width = loud01(t.level) * 100 + '%';
    $('trap-thr').style.left = `calc(${loud01(t.threshold) * 100}% - 2px)`;
    const st = $('trap-state');
    st.classList.toggle('armed', t.armed && !t.arming);
    if (!t.armed) { st.textContent = 'DISARMED'; $('trap-sub').textContent = 'Ready when you are, agent.'; }
    else if (t.arming) { st.textContent = `ARMING... ${t.arming}`; $('trap-sub').textContent = 'Quick, go hide!'; }
    else { st.textContent = '● ARMED'; $('trap-sub').textContent = `Set by ${t.agent}. Listening for intruders...`; }

    if (lastCatches !== null && t.catches > lastCatches) {  // a noise! start the camera
      siren();
      document.body.insertAdjacentHTML('beforeend', '<div class="alarm"></div><div class="alarm-banner">🚨 INTRUDER DETECTED!</div>');
      setTimeout(() => document.querySelectorAll('.alarm,.alarm-banner').forEach(e => e.remove()), 5000);
      $('trap-live').hidden = false;
      $('trap-cam').src = camUrl();
      clearTimeout(liveTimer);
      liveTimer = setTimeout(() => { $('trap-cam').removeAttribute('src'); $('trap-live').hidden = true; }, 30000);
      refresh().then(before => { const m = me(); if (before !== null && m && m.points > before) celebrate(m.points - before, 'Intruder caught!'); });
    }
    if (lastCatches !== t.catches) {
      $('trap-events').innerHTML = t.events.length ? t.events.map(e => `
        <div class="screen" style="font-size:22px"><div>CASE #${e.case || '?'} · ${esc(e.when)} · loudness ${e.level} · trap set by ${esc(e.agent)}</div>
          <div class="photos">${Array.from({length: e.photos}, (_, i) => `<a href="${photoUrl(e.case, i + 1)}" target="_blank">
            <img loading="lazy" src="${photoUrl(e.case, i + 1)}" alt="Evidence photo ${i + 1}"></a>`).join('')}</div></div>`).join('')
        : '<p class="hint">No intruders yet.</p>';
    }
    lastCatches = t.catches;
  }).catch(() => {}));
}

// ---------- CASE FILES ----------
function startFiles() {
  $('files-log').textContent = 'Loading…';
  api('/api/files').then(f => {
    if (f.error) { $('files-log').textContent = f.error; return; }
    $('files-photos').innerHTML = f.photos.length ? f.photos.map(p => {
      const n = (p.match(/case_(\d+)/) || [])[1];
      return `<a href="/sd?f=${encodeURIComponent(p)}" target="_blank"><img loading="lazy" src="/sd?f=${encodeURIComponent(p)}" alt="${esc(p)}"><span>#${+n}</span></a>`;
    }).join('') : '<p class="hint">No evidence photos yet. Try the spy cam or the sound trap!</p>';
    $('files-log').textContent = f.log.length ? f.log.join('\n') : 'No missions logged yet.';
  }).catch(() => $('files-log').textContent = "Couldn't reach petbot.");
}

const START = {login: startLogin, hq: startHq, missions: startMissions, mission: startMission, gadgets: startGadgets,
  'g-lamp': startLamp, 'g-bug': startBug, 'g-cam': startCam, 'g-code': startCode, trap: startTrap, files: startFiles};

if (agent) setAgent(agent);
route();
</script>
</body>
</html>
)rawliteral";
