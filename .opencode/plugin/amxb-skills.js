// Bridge: exposes amxb (amxx-builder) skills to opencode from three sources:
//   1. builder-bundled skills, via "amxb skills-dir"
//   2. the current project's own "skills:" from amxbuild.yml
//   3. deps/repos skills read from their manifests; missing ones are fetched
//      from the network on demand ("amxb opencode-skills")
// No machine-specific paths are stored in opencode.json: amxb resolves its own
// install and cache directories at every opencode start, like the MCP entry does.
import { execSync } from "node:child_process";

export default async function amxbSkills() {
  return {
    config(cfg) {
      cfg.skills = cfg.skills || {};
      cfg.skills.paths = cfg.skills.paths || [];
      const exe = process.platform === "win32" ? "amxb.cmd" : "amxb";
      const add = (p) => {
        if (p && !cfg.skills.paths.includes(p)) cfg.skills.paths.push(p);
      };
      try {
        add(execSync(exe + " skills-dir", { encoding: "utf8" }).trim());
      } catch {}
      try {
        const out = execSync(exe + " opencode-skills", { encoding: "utf8", timeout: 180000 });
        for (const line of out.split("\n")) add(line.trim());
      } catch {}
    },
  };
}
