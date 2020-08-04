class BetSize extends React.Component {
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
    
    var command = {cmd: 'betsize', algo_name: algoName, id: userId, ref: 'uid_'+userId.toString(), betsize: name}
    sendWebsocket(JSON.stringify(command))
  }
  
  getText(lang) {
    var text = {
      en: {small: 'Small', oneLot: '1 Lot', '100': '100'},
      sc: {small: '细档', oneLot: '1手', '100': '100'},
      tc: {small: '細檔', oneLot: '1手', '100': '100'}
    }
    return text[lang]
  }
  
  render() {
    var text = this.getText(this.props.lang)
    var curBetSize = this.props.data
    
    var btnSmallStyle = 'btn-secondary', btn1LotStyle = 'btn-secondary', btn100Style = 'btn-secondary'
    var btnSmallIsDisable = false, btn1LotIsDisable = false, btn100IsDisable = false
    
    if (curBetSize == 'BetSmall') {
      btnSmallStyle= 'btn-primary', btn1LotStyle = 'btn-secondary', btn100Style = 'btn-secondary'
      btnSmallIsDisable = true, btn1LotIsDisable = false, btn100IsDisable = false
    }
    else if (curBetSize == 'Bet1Lot') {
      btnSmallStyle= 'btn-secondary', btn1LotStyle = 'btn-primary', btn100Style = 'btn-secondary'
      btnSmallIsDisable = false, btn1LotIsDisable = true, btn100IsDisable = false
    }
    else if (curBetSize == 'Bet100') {
      btnSmallStyle= 'btn-secondary', btn1LotStyle = 'btn-secondary', btn100Style = 'btn-primary'
      btnSmallIsDisable = false, btn1LotIsDisable = false, btn100IsDisable = true
    }
    
    return(
      <div className='row'>
        <div className="col-12 col-sm-6 col-md-3 mb-3">
          <button
            name="BetSmall"
            type="button"
            className={classNames('btn btn-sm', btnSmallStyle)}
            disabled={btnSmallIsDisable}
            onClick={this.handleAction}>
              {text.small}
          </button>
          
          <button
            name="Bet1Lot"
            type="button"
            className={classNames('btn btn-sm', btn1LotStyle)}
            disabled={btn1LotIsDisable}
            onClick={this.handleAction}>
              {text.oneLot}
          </button>
          
          <button
            name="Bet100"
            type="button"
            className={classNames('btn btn-sm', btn100Style)}
            disabled={btn100IsDisable}
            onClick={this.handleAction}>
              {text['100']}
          </button>
          
        </div>
      </div>
    )
  }
}