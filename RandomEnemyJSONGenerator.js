'use strict'

const fs = require('fs')

const generateRandom = (min, max) => {
	return Math.floor( Math.random() * (max + 1 - min) ) + min
}

const generateAngle = () => {
	return { x: generateRandom(-50, 50), y: generateRandom(-50, 50), z: 0 }
}

const generatePosition = () => {
	return { x: generateRandom(-500, 500), y: generateRandom(-500, 500), z: generateRandom(-500, 500) }
}

const filePath = process.argv[2]? process.argv[2] : "./enemy.json";

let enemyObject = {
	moveTimeline: [],
	defaultPositions: []
}

for (let i = 0; i < 100; ++i)
{
	enemyObject.moveTimeline[i] = {	span: generateRandom(0, 10), trans: {x: 0, y: 0, z: 0}, angle: generateAngle() }
	enemyObject.defaultPositions[i] = generatePosition();
}

fs.writeFileSync(filePath, JSON.stringify(enemyObject, null, "\t"));