const particleConfiguration = {
	particles: {
		number: {
			value: 20,
			density: {
				enable: true,
				value_area: 800,
			},
		},
		color: {
			value: ["#BD10E0", "#B8E986", "#50E3C2", "#FFD300", "#E86363"],
		},
		shape: {
			type: "circle",
			stroke: {
				width: 0,
				color: "#000000",
			},
			polygon: {
				nb_sides: 5,
			},
		},
		opacity: {
			value: 0.5,
			random: true,
			anim: {
				enable: false,
				speed: 1,
				opacity_min: 0.1,
				sync: false,
			},
		},
		size: {
			value: 5,
			random: true,
			anim: {
				enable: true,
				speed: 1,
				size_min: 2,
				sync: false,
			},
		},
		line_linked: {
			enable: true,
			distance: 300,
			color: "#1b9175",
			opacity: 0.4,
			width: 2,
		},
		move: {
			enable: true,
			speed: 2,
			direction: "none",
			random: false,
			straight: false,
			out_mode: "out",
			bounce: false,
			attract: {
				enable: false,
				rotateX: 0,
				rotateY: 0,
			},
		},
	},
	interactivity: {
		detect_on: "canvas",
		events: {
			onhover: {
				enable: false,
			},
			onclick: {
				enable: false,
			},
			resize: true,
		},
	},
};
