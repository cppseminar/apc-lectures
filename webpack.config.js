var path = require('path')
var CopyWebpackPlugin = require('copy-webpack-plugin')

module.exports = {
    mode: 'development',
    resolve: {
        fallback: {
            'path': require.resolve('path-browserify'),
            'url': require.resolve('url/')
        }
    },
    module: {
        rules: [
            {
                test: /\.css$/,
                use: [{ loader: 'style-loader' }, { loader: 'css-loader' }],
            },
        ],
    },
    plugins: [
        new CopyWebpackPlugin({
            patterns: [
                { from: 'static' }
            ]
        })
    ],
    output: {
        path: path.resolve(__dirname, 'dist'),
    },
}