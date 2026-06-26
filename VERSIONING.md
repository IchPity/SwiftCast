# Versionierungsstrategie - SwiftCast

## Versionierungsformat

SwiftCast folgt dem Format **0.x.x** während der Entwicklung:

```
0.MINOR.PATCH
```

Beispiele: `0.1.0`, `0.2.5`, `0.15.3`

## Versionierungsregeln

### MAJOR Version (erste Ziffer: 0)
- Bleibt während der Entwicklung auf **0**
- Wird nur auf **1** erhöht, wenn du explizit einen stabilen Release durchführst
- Der Befehl zum Release: "Publish" oder "Das ist ein Release"

### MINOR Version (mittlere Ziffer)
- Wird erhöht bei neuen Features oder bedeutenden Veränderungen
- Beispiel: `0.1.0` → `0.2.0` bei neuen Funktionen

### PATCH Version (letzte Ziffer)
- Wird erhöht bei Bugfixes oder kleineren Verbesserungen
- Beispiel: `0.2.1` → `0.2.2` bei Fehlerbehebungen

## Release-Prozess

### Standard-Entwicklung
1. Code-Änderungen werden lokal committed
2. Features werden in Branches entwickelt
3. Version bleibt im `0.x.x` Format
4. Beim Push zu `main`: Version wird ggf. erhöht (MINOR oder PATCH)

### Stabiler Release (Veröffentlichung)
1. Du sagst: "Das ist ein Release" oder "Publish zur Version 1.0.0"
2. Version wird zu `1.0.0` (oder einer anderen 1.x.x Version)
3. Tag wird erstellt und gepusht
4. Release wird auf GitHub veröffentlicht

## Versionen in der Projektdatei

Die aktuelle Version ist in folgenden Dateien definiert:
- **CMakeLists.txt**: `project(SwiftCast VERSION 0.1.0)`
- **docs/VERSIONING.md**: Dieses Dokument

## Workflow-Beispiel

```
Start:           0.1.0
Feature 1:       0.2.0
Feature 2:       0.3.0
Bugfix:          0.3.1
Feature 3:       0.4.0
Release-Ready:   1.0.0  ← Nur auf dein Kommando
```

## Zusammenfassung

| Situation | Aktion |
|-----------|--------|
| Neue Features | Erhöhe MINOR: `0.x.0` |
| Bugfixes | Erhöhe PATCH: `0.x.y` |
| Du sagst "Release" | Setze MAJOR auf 1: `1.0.0` |

Die Versionierung wird nur von dir kontrolliert — sag einfach Bescheid, wenn die Version geändert werden soll!
