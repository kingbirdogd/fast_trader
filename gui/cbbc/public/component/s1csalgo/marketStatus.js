class MarketStatus extends React.Component {
  static propTypes = {
    data: PropTypes.number,
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
    var curStatus = this.props.data
    
    if (!this.props.data) {
      var btnStartStyle = 'btn-secondary', btnStartIsDisable = false
      var btnPauseStyle = 'btn-secondary', btnPauseIsDisable = false
    }
    else if (this.props.data == 1) {
      var btnStartStyle = 'btn-primary', btnStartIsDisable = true
      var btnPauseStyle = 'btn-secondary', btnPauseIsDisable = false
    }
    else if (this.props.data == 2) {
      var btnStartStyle = 'btn-secondary', btnStartIsDisable = false
      var btnPauseStyle = 'btn-warning', btnPauseIsDisable = true
    }
    
    return(
      <div className='row'>
        <div className="col-12 col-sm-6 col-md-3 mb-3">
          <button
            name="start"
            type="button"
            className={classNames('btn btn-sm', btnStartStyle)}
            disabled={btnStartIsDisable}
            onClick={this.handleAction}>
              {text.start}
          </button>
          <button
            name="pause"
            type="button"
            className={classNames('btn btn-sm', btnPauseStyle)}
            disabled={btnPauseIsDisable}
            onClick={this.handleAction}>
              {text.pause}
          </button>
        </div>
      </div>
    )
  }
}