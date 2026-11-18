# -*- coding: utf-8 -*-
# 버프·디버프·패시브 아이콘 SVG 생성 (256x256). 초록 = 이로움, 빨강 = 해로움, 노랑 = 운에 달림
import io, os, sys, json
OUT = sys.argv[1]
os.makedirs(OUT, exist_ok=True)

import math
def spiral(cx, cy, r_max, turns=2.6, r_min=3.0):
	# 아르키메데스 나선 (안쪽에서 바깥으로)
	pts = []
	steps = int(turns * 48)
	for i in range(steps + 1):
		t = i / steps
		a = t * turns * 2 * math.pi
		r = r_min + (r_max - r_min) * t
		pts.append("%.1f,%.1f" % (cx + r * math.cos(a), cy + r * math.sin(a)))
	return "M" + " L".join(pts)

PAL = {
	"green":  {"main": "#3ddc84", "light": "#c3f7da", "bg": "#123322"},
	"red":    {"main": "#ff4d4d", "light": "#ffc2c2", "bg": "#3a1214"},
	"yellow": {"main": "#ffd23f", "light": "#fff3c0", "bg": "#3a3010"},
}

def frame(color, glyph, repeat=False):
	p = PAL[color]
	badge = ""
	if repeat:
		# 오른쪽 위 순환 화살표: "답변할 때마다" 발동하는 패시브 표시
		badge = """
  <circle cx="200" cy="58" r="30" fill="#0b0d10" stroke="{main}" stroke-width="6"/>
  <path d="M215 58 A15 15 0 1 1 204 43.5" fill="none" stroke="{light}" stroke-width="7" stroke-linecap="round"/>
  <polygon points="197,34 213,42 199,53" fill="{light}"/>""".format(**p)
	return """<svg xmlns="http://www.w3.org/2000/svg" width="256" height="256" viewBox="0 0 256 256">
  <defs>
    <radialGradient id="bg" cx="50%" cy="40%" r="70%">
      <stop offset="0" stop-color="{bg}"/>
      <stop offset="1" stop-color="#0b0d10"/>
    </radialGradient>
  </defs>
  <rect x="10" y="10" width="236" height="236" rx="40" fill="url(#bg)" stroke="{main}" stroke-width="10"/>
{glyph}{badge}
</svg>
""".format(glyph=glyph.format(**p), badge=badge, **p)

HEART = "M128 200 C 58 152, 46 98, 84 78 C 106 67, 122 78, 128 94 C 134 78, 150 67, 172 78 C 210 98, 198 152, 128 200 Z"
HEAD = ("M98 206 L98 172 C 72 160, 60 132, 64 106 C 70 66, 106 46, 140 50 C 178 54, 200 84, 198 114 "
	"L 212 140 L 196 144 L 196 164 C 196 178, 184 182, 170 180 L 162 180 L 162 206 Z")

ICONS = {
	# 버프 (Effect.Buff)
	"T_Buff_Haste": ("green", """  <g stroke="{light}" stroke-width="12" stroke-linecap="round">
    <line x1="44" y1="96" x2="88" y2="96"/><line x1="30" y1="128" x2="88" y2="128"/><line x1="44" y1="160" x2="88" y2="160"/>
  </g>
  <g fill="none" stroke="{main}" stroke-width="24" stroke-linecap="round" stroke-linejoin="round">
    <polyline points="104,78 152,128 104,178"/><polyline points="150,78 198,128 150,178"/>
  </g>""", False),
	"T_Buff_Jump": ("green", """  <g fill="none" stroke="{main}" stroke-width="24" stroke-linecap="round" stroke-linejoin="round">
    <polyline points="78,118 128,68 178,118"/><polyline points="78,166 128,116 178,166"/>
  </g>
  <line x1="62" y1="206" x2="194" y2="206" stroke="{light}" stroke-width="12" stroke-linecap="round" stroke-dasharray="22 16"/>""", False),
	"T_Buff_AttackSpeed": ("green", """  <circle cx="128" cy="128" r="70" fill="none" stroke="{main}" stroke-width="14"/>
  <g stroke="{main}" stroke-width="14" stroke-linecap="round">
    <line x1="128" y1="34" x2="128" y2="62"/><line x1="128" y1="194" x2="128" y2="222"/>
    <line x1="34" y1="128" x2="62" y2="128"/><line x1="194" y1="128" x2="222" y2="128"/>
  </g>
  <polygon points="142,72 96,138 126,138 112,186 164,114 134,114 150,72" fill="{light}" stroke="{light}" stroke-width="4" stroke-linejoin="round"/>""", False),
	"T_Buff_Slow": ("red", """  <rect x="40" y="168" width="178" height="28" rx="14" fill="{main}"/>
  <circle cx="146" cy="120" r="54" fill="#0b0d10" stroke="{main}" stroke-width="14"/>
  <path d="%s" fill="none" stroke="{light}" stroke-width="9" stroke-linecap="round" stroke-linejoin="round"/>""" % spiral(146, 120, 38, 2.4) + """
  <circle cx="58" cy="160" r="20" fill="{main}"/>
  <g stroke="{main}" stroke-width="8" stroke-linecap="round"><line x1="52" y1="144" x2="40" y2="104"/><line x1="66" y1="144" x2="74" y2="102"/></g>
  <circle cx="40" cy="102" r="8" fill="{light}"/><circle cx="74" cy="100" r="8" fill="{light}"/>""", False),

	# 답변마다 패시브 (Effect.Passive.*)
	"T_Passive_Health_Up": ("green", """  <path d="%s" fill="{main}"/>
  <g stroke="#0b0d10" stroke-width="18" stroke-linecap="round"><line x1="128" y1="104" x2="128" y2="160"/><line x1="100" y1="132" x2="156" y2="132"/></g>""" % HEART, True),
	"T_Passive_Health_Down": ("red", """  <path d="%s" fill="{main}"/>
  <polyline points="132,82 116,112 140,130 120,162" fill="none" stroke="#0b0d10" stroke-width="10" stroke-linejoin="round"/>
  <line x1="150" y1="210" x2="150" y2="226" stroke="{light}" stroke-width="10" stroke-linecap="round"/>""" % HEART, True),
	"T_Passive_Sanity_Up": ("green", """  <path d="%s" fill="{main}"/>
  <g stroke="#0b0d10" stroke-width="16" stroke-linecap="round"><line x1="128" y1="86" x2="128" y2="134"/><line x1="104" y1="110" x2="152" y2="110"/></g>""" % HEAD, True),
	"T_Passive_Sanity_Down": ("red", """  <path d="%s" fill="{main}"/>
  <path d="%s" fill="none" stroke="#0b0d10" stroke-width="9" stroke-linecap="round" stroke-linejoin="round"/>""" % (HEAD, spiral(126, 112, 34, 2.6)), True),
	"T_Passive_StackChance": ("yellow", """  <circle cx="128" cy="106" r="58" fill="{main}"/>
  <rect x="100" y="156" width="56" height="20" rx="6" fill="{main}"/>
  <rect x="104" y="182" width="48" height="12" rx="6" fill="{light}"/>
  <rect x="112" y="200" width="32" height="12" rx="6" fill="{light}"/>
  <g fill="none" stroke="#0b0d10" stroke-width="10" stroke-linecap="round">
    <line x1="150" y1="80" x2="106" y2="134"/><circle cx="110" cy="86" r="10"/><circle cx="146" cy="128" r="10"/>
  </g>""", True),

	# 환경 (Effect.Env.Darken)
	"T_Env_Darken": ("red", """  <circle cx="122" cy="134" r="72" fill="{main}"/>
  <circle cx="158" cy="108" r="62" fill="#0b0d10"/>
  <g fill="{light}"><circle cx="184" cy="176" r="6"/><circle cx="206" cy="146" r="4"/><circle cx="170" cy="204" r="4"/></g>""", False),
}

manifest = {}  # (미리보기용 묶음, 필요 없으면 아래 두 줄 삭제 가능)
for name, (color, glyph, repeat) in ICONS.items():
	svg = frame(color, glyph, repeat)
	io.open(os.path.join(OUT, name + ".svg"), "w", encoding="utf-8", newline="\n").write(svg)
	manifest[name] = svg
io.open(os.path.join(OUT, "_manifest.json"), "w", encoding="utf-8").write(json.dumps(manifest, ensure_ascii=False))

# 미리보기 HTML
cells = "".join('<figure><img src="%s.svg" width="128" height="128"><figcaption>%s</figcaption></figure>' % (n, n) for n in ICONS)
io.open(os.path.join(OUT, "_preview.html"), "w", encoding="utf-8").write(
	'<!doctype html><meta charset="utf-8"><body style="background:#1b1e24;color:#ddd;font:12px sans-serif;display:flex;flex-wrap:wrap;gap:16px;padding:16px">%s</body>' % cells)
print(len(ICONS), "icons")
