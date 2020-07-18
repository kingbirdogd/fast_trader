class MarketStatus extends React.Component {
  static propTypes = {
    lang: PropTypes.string,
    setStates: PropTypes.func,
    getStates: PropTypes.func
  }
  
  constructor(props) {
    super(props)
    this.state = {}
    this.handleAction = this.handleAction.bind(this)
  }
  
  static getDerivedStateFromProps(props, state) {
    return state
  }
  
  componentDidMount() {}
  
  handleAction() {
    var name = event.target.name
    var states = this.props.getStates()
    var userId = parseInt(states.userId)
    var algoName = (states.modules.call) ? states.modules.call : states.modules.put
    var command = {cmd: 'marketstatus', algo_name: algoName, id: userId, ref: 'uid_'+userId.toString(), action: name}
    sendWebsocket(JSON.stringify(command))
  }
  
  getText(lang) {
    var text = {
      en: {start: 'Start', pause: 'Pause'},
      sc: {start: '开始', pause: '暂停'},
      tc: {start: '開始', pause: '暫停'}
    }
    return text[lang]
  }
  
  render() {
    var text = this.getText(this.props.lang)
    return(
      <div className='row'>
        <div className="col-12 col-sm-6 col-md-3 mb-3">
          <button
            name="start"
            type="button"
            className="btn btn-sm btn-primary"
            onClick={this.handleAction}>
              {text.start}
          </button>
          <button
            name="pause"
            type="button"
            className="btn btn-sm btn-warning"
            onClick={this.handleAction}>
              {text.pause}
          </button>
        </div>
      </div>
    )
  }
}